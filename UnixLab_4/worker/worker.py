import pika
import json
import subprocess

connection = pika.BlockingConnection(pika.ConnectionParameters('rabbitmq'))
channel = connection.channel()

channel.queue_declare(queue='tasks')
channel.queue_declare(queue='results')

def check_prime(n):
    result = subprocess.run(["/app/is_prime", str(n)], capture_output=True, text=True)
    return result.stdout.strip() == "true"

def callback(ch, method, properties, body):
    data = json.loads(body)
    task_id = data['task_id']
    number = data['number']
    
    is_prime = check_prime(number)
    response = {'task_id': task_id, 'number': number, 'is_prime': is_prime}

    channel.basic_publish(
        exchange='',
        routing_key='results',
        body=json.dumps(response)
    )

    ch.basic_ack(method.delivery_tag)

channel.basic_consume(queue='tasks', on_message_callback=callback, auto_ack=False)
print('Worker запущен, ждет задач...')
channel.start_consuming()
