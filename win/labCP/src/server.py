import os
import sys
import json
import time
import threading
import numpy as np
import pandas as pd

from math import ceil
from collections import defaultdict
from datetime import datetime
from concurrent.futures import ThreadPoolExecutor, as_completed

class DAGProcessor:
    def __init__(self, config_path, max_workers=4):
        self.config = self.load_config(config_path)
        self.part_files = []
        self.partial_results = defaultdict(dict)
        self.execution_failed = False
        self.error_message = ""
        self.max_workers = max_workers
        self.lock = threading.Lock()
        self.completed_jobs = set()
        self.in_progress = set()
        self.print_lock = threading.Lock()
        
    def load_config(self, path):
        with open(path) as f:
            return json.load(f)
        
    def format_result(self, result):
        """Форматирует вывод результатов для лучшей читаемости"""
        formatted = []
        for key, value in result.items():
            if isinstance(value, (float, np.float64, np.int64)):
                py_value = float(value) if isinstance(value, (float, np.float64)) else int(value)
                if isinstance(py_value, float):
                    formatted.append(f"{key}: {py_value:.2f}")
                else:
                    formatted.append(f"{key}: {py_value}")
            else:
                formatted.append(f"{key}: {value}")
        return "\n".join(formatted)
    
    def safe_print(self, *args, **kwargs):
        with self.lock:
            print(*args, **kwargs)
    
    def split_csv(self, source, output_parts, parts):
        try:
            time.sleep(5)
            os.makedirs(output_parts, exist_ok=True)
            df = pd.read_csv(source)
            chunk_size = ceil(len(df) / parts)
            self.part_files = []
            
            for i in range(parts):
                start = i * chunk_size
                end = min((i+1)*chunk_size, len(df))
                part_path = f"{output_parts}/part_{i}.csv"
                df.iloc[start:end].to_csv(part_path, index=False)
                self.part_files.append(part_path)
            
            return self.part_files
        except Exception as e:
            with self.lock:
                self.execution_failed = True
                self.error_message = f"Ошибка при разделении CSV: {str(e)}"
            raise

    def analyze_gender_avg(self, input_path):
        try:
            time.sleep(5)
            df = pd.read_csv(input_path)
            
            if 'gender' not in df.columns or 'transaction_amount' not in df.columns:
                raise ValueError("Отсутствуют необходимые колонки в данных")
                
            gender_stats = df.groupby('gender')['transaction_amount'].agg(['sum', 'count'])
            
            return {
                'male_sum': gender_stats.loc['Male', 'sum'] if 'Male' in gender_stats.index else 0,
                'male_count': gender_stats.loc['Male', 'count'] if 'Male' in gender_stats.index else 0,
                'female_sum': gender_stats.loc['Female', 'sum'] if 'Female' in gender_stats.index else 0,
                'female_count': gender_stats.loc['Female', 'count'] if 'Female' in gender_stats.index else 0,
                'source_file': os.path.basename(input_path)
            }
        except Exception as e:
            with self.lock:
                self.execution_failed = True
                self.error_message = f"Ошибка при анализе данных: {str(e)}"
            raise

    def combine_gender_stats(self, results):
        try:
            time.sleep(5)
            total_male = sum(r['male_sum'] for r in results)
            total_female = sum(r['female_sum'] for r in results)
            count_male = sum(r['male_count'] for r in results)
            count_female = sum(r['female_count'] for r in results)
            
            return {
                'avg_male': total_male / count_male if count_male > 0 else 0,
                'avg_female': total_female / count_female if count_female > 0 else 0,
                'total_male': total_male,
                'total_female': total_female,
                'count_male': count_male,
                'count_female': count_female
            }
        except Exception as e:
            with self.lock:
                self.execution_failed = True
                self.error_message = f"Ошибка при объединении результатов: {str(e)}"
            raise
    
    def execute_job(self, job_id):
        if self.execution_failed:
            return []

        try:
            job = self.config["dag"][job_id]
            next_jobs = job.get("edges", [])
            start_time = time.time()
            current_time = datetime.now().strftime("%H:%M:%S")
            
            # Собираем ВЕСЬ вывод в один буфер
            output_buffer = []
            
            # Заголовок задания
            output_buffer.append(f"\n\033[1m[{current_time}] {job_id} ({job['name']})\033[0m")
            output_buffer.append(f"\033[36mТип:\033[0m {job['type']}")
            
            if job["type"] == "csv_split":
                output_buffer.append(f"\033[36mДействие:\033[0m Разделение {job['source']} на {job['parts']} частей")
                parts = self.split_csv(
                    source=job["source"],
                    output_parts=job["target"],
                    parts=int(job["parts"])
                )
                output_buffer.append(f"\033[32mРезультат:\033[0m Создано {len(parts)} файлов:")
                for i, part in enumerate(parts):
                    output_buffer.append(f"  - part_{i}.csv")
                    
            elif job["type"] == "analyze_gender_avg":
                output_buffer.append(f"\033[36mДействие:\033[0m Анализ файла {job['input']}")
                result = self.analyze_gender_avg(job["input"])
                with self.lock:
                    self.partial_results[job_id] = result
                
                output_buffer.append("\033[32mРезультат анализа:\033[0m")
                output_buffer.append(f"  - Мужчины: {result['male_count']} записей, сумма: {float(result['male_sum']):.2f}")
                output_buffer.append(f"  - Женщины: {result['female_count']} записей, сумма: {float(result['female_sum']):.2f}")
                
            elif job["type"] == "combine_stats":
                sources = []
                for src_job_id, src_job in self.config["dag"].items():
                    for edge in src_job.get("edges", []):
                        if edge["to"] == job_id:
                            sources.append(src_job_id)
                
                output_buffer.append(f"\033[36mДействие:\033[0m Объединение результатов от {', '.join(sources)}")
                with self.lock:
                    results_to_combine = [self.partial_results[src] for src in sources]
                combined = self.combine_gender_stats(results_to_combine)
                with self.lock:
                    self.partial_results[job_id] = combined
                
                output_buffer.append("\033[32mИтоговая статистика:\033[0m")
                output_buffer.append(f"  - Средний платеж (М): {combined['avg_male']:.2f}")
                output_buffer.append(f"  - Средний платеж (Ж): {combined['avg_female']:.2f}")
                output_buffer.append(f"  - Всего записей (М): {combined['count_male']}")
                output_buffer.append(f"  - Всего записей (Ж): {combined['count_female']}")
                
            elif job["type"] == "sending_result":
                with self.lock:
                    data_6 = self.partial_results.get("job_6", {})
                    data_7 = self.partial_results.get("job_7", {})
                
                output_buffer.append("\n\033[1;35m=== ФИНАЛЬНЫЙ ОТЧЕТ ===\033[0m")
                output_buffer.append(f"\033[1mСредний платеж:\033[0m")
                output_buffer.append(f"  - Мужчины: ${(data_6.get('avg_male', 0) + data_7.get('avg_male', 0)) / 2:.2f}")
                output_buffer.append(f"  - Женщины: ${(data_6.get('avg_female', 0) + data_7.get('avg_female', 0)) / 2:.2f}")
                output_buffer.append(f"\033[1mВсего записей:\033[0m")
                output_buffer.append(f"  - Мужчины: {int(data_6.get('count_male', 0)) + int(data_7.get('count_male', 0))}")
                output_buffer.append(f"  - Женщины: {int(data_6.get('count_female', 0)) + int(data_7.get('count_female', 0))}")
                output_buffer.append("\033[1;35m" + "="*30 + "\033[0m")
            
            elapsed = time.time() - start_time
            output_buffer.append(f"\033[36mВремя выполнения:\033[0m {elapsed:.2f} сек")
            
            # Выводим ВЕСЬ результат задачи одной операцией
            self.safe_print("\n".join(output_buffer))
            
            return next_jobs
            
        except Exception as e:
            with self.lock:
                self.execution_failed = True
                self.error_message = f"Ошибка в джобе {job_id}: {str(e)}"
            self.safe_print(f"\n\033[1;31m[ОШИБКА] {self.error_message}\033[0m")
            return []

    def run_dag(self, start_job="job_1"):
        with ThreadPoolExecutor(max_workers=self.max_workers) as executor:
            self.safe_print("\033[1;34m=== Начало выполнения DAG ===\033[0m")
            self.safe_print(f"Максимальное параллельных задач: {self.max_workers}\n")
            
            queue = [start_job]
            futures = {}
            
            while queue and not self.execution_failed:
                # Запуск задач
                while len(self.in_progress) < self.max_workers and queue and not self.execution_failed:
                    job_id = queue.pop(0)
                    
                    if job_id in self.completed_jobs:
                        continue
                        
                    if job_id in self.in_progress:
                        continue
                        
                    # Проверка зависимостей
                    dependencies_ready = True
                    for src_job_id, src_job in self.config["dag"].items():
                        for edge in src_job.get("edges", []):
                            if edge["to"] == job_id and src_job_id not in self.completed_jobs:
                                dependencies_ready = False
                                break
                    
                    if not dependencies_ready:
                        queue.append(job_id)
                        continue
                        
                    with self.lock:
                        self.in_progress.add(job_id)
                    future = executor.submit(self.execute_job, job_id)
                    futures[future] = job_id
                
                # Обработка завершенных задач
                for future in as_completed(futures):
                    job_id = futures.pop(future)
                    with self.lock:
                        self.in_progress.remove(job_id)
                        self.completed_jobs.add(job_id)
                    
                    if not self.execution_failed:
                        try:
                            next_jobs = future.result()
                            queue.extend([edge["to"] for edge in next_jobs 
                                        if edge["to"] not in self.completed_jobs and
                                        edge["to"] not in self.in_progress])
                        except Exception as e:
                            with self.lock:
                                self.execution_failed = True
                                self.error_message = str(e)
            
            if self.execution_failed:
                self.safe_print("\n\033[1;31m!!! ВЫПОЛНЕНИЕ DAG ПРЕРВАНО ИЗ-ЗА ОШИБКИ !!!\033[0m")
                self.safe_print(f"Причина: {self.error_message}")
                sys.exit(1)
            else:
                self.safe_print("\n\033[1;32m=== ВЫПОЛНЕНИЕ DAG УСПЕШНО ЗАВЕРШЕНО ===\033[0m")

# Загрузка конфигурационного dag'a
def load_dag_from_file(file_path):
    try:
        with open(file_path, 'r', encoding='utf-8') as f:
            data = json.load(f)

        dag = defaultdict(list)
        node_attrs = {}

        for node_id, node_data in data['dag'].items():
            node_attrs[node_id] = {
                'name': node_data['name']
            }

            for edge in node_data['edges']:
                dag[node_id].append(edge['to'])

        return dag, node_attrs
    except FileNotFoundError:
        print(f"Error: file {file_path} not found")
        return {}, {}
    except json.JSONDecodeError:
        print("Error: file has uncorrect JSON")
    return {}, {}

# Поиск в глубину (универсальный)
def dfs(graph, start, visited=None, recursion_stack=None, detect_cycle=False):
    if visited is None:
        visited = set()
    if detect_cycle and recursion_stack is None:
        recursion_stack = set()

    if detect_cycle and start in recursion_stack:
        return visited, True
    if start in visited:
        return visited, False

    visited.add(start)
    if detect_cycle:
        recursion_stack.add(start)

    for neighbor in graph.get(start, []):
        visited, has_cycle = dfs(graph, neighbor, visited, recursion_stack, detect_cycle)
        if detect_cycle and has_cycle:
            return visited, True

    if detect_cycle:
        recursion_stack.remove(start)
    return visited, False

# Проверка на ацикличность
def has_cycle(graph):
    visited = set()
    for node in graph:
        if node not in visited:
            _, cycle_found = dfs(graph, node, visited, None, detect_cycle=True)
            if cycle_found:
                return True
    return False

# Проверка на наличие одной компоненты связности
def is_weakly_connected(graph):
    undirected = defaultdict(list)
    for u in graph:
        for v in graph[u]:
            undirected[u].append(v)
            undirected[v].append(u)

    if not undirected:
        return True

    start_node = next(iter(undirected))
    visited, _ = dfs(undirected, start_node)
    return len(visited) == len(undirected)

# Валидность dag'a
def is_valid_dag(graph):
    if not graph:
        return False

    all_nodes = set(graph.keys())
    for edges in graph.values():
        all_nodes.update(edges)

    in_degree = defaultdict(int)
    out_degree = defaultdict(int)

    for u in graph:
        out_degree[u] = len(graph[u])
        for v in graph[u]:
            in_degree[v] += 1

    sources = [u for u in all_nodes if in_degree[u] == 0]
    sinks = [u for u in all_nodes if out_degree[u] == 0]

    return (
        not has_cycle(graph)
        and is_weakly_connected(graph)
        and len(sources) >= 1
        and len(sinks) == 1
    )


if __name__ == "__main__":
    dag, node_attrs = load_dag_from_file('dag.json')
    
    if is_valid_dag(dag):
        print("Graph struct is:")
        for node, edges in dag.items():
            print(f"{node} ({node_attrs[node]['name']}) -> {edges}")

        processor = DAGProcessor("dag.json", max_workers=4)
        processor.run_dag()
    else:
        print("Error: Uncorrect DAG")
        sys.exit(1)
