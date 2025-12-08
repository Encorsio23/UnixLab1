import pika
import json
import uuid
from flask import Flask, request, jsonify

app = Flask(__name__)

connection = pika.BlockingConnection(pika.ConnectionParameters('rabbitmq'))
channel = connection.channel()

channel.queue_declare(queue='tasks')
channel.queue_declare(queue='results')

def send_task(number):
    task_id = str(uuid.uuid4())

    channel.basic_publish(
        exchange='',
        routing_key='tasks',
        properties=pika.BasicProperties(correlation_id=task_id),
        body=json.dumps({'task_id': task_id, 'number': number})
    )

    return task_id

def get_result(task_id):
    response = None

    def on_response(ch, method, props, body):
        nonlocal response
        data = json.loads(body)
        if data['task_id'] == task_id:
            response = data
            ch.basic_ack(method.delivery_tag)
            ch.stop_consuming()

    channel.basic_consume(queue='results', on_message_callback=on_response, auto_ack=False)
    channel.start_consuming()

    return response

@app.route('/check_prime', methods=['POST'])
def check_prime():
    data = request.get_json()
    number = data.get('number')

    task_id = send_task(number)
    result = get_result(task_id)

    return jsonify(result)

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5000)
