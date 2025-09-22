import pandas as pd
import numpy as np
from faker import Faker
import random

fake = Faker()

data = []
for i in range(1, 10001):
    data.append({
        'id': i,
        'first_name': fake.first_name(),
        'last_name': fake.last_name(),
        'email': fake.email(),
        'gender': random.choice(['Male', 'Female']),
        'ip_address': fake.ipv4(),
        'transaction_amount': round(random.uniform(5, 500), 2),
        'transaction_date': fake.date_between(start_date='-1y', end_date='today')
    })

df = pd.DataFrame(data)

df.to_csv('personal_data.csv', index=False)
print("Файл test_data.csv создан (5000 строк)")