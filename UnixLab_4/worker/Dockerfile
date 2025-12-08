FROM python:3.9
WORKDIR /app
COPY is_prime.cpp .
RUN g++ -o is_prime is_prime.cpp
COPY worker.py .
RUN pip install pika
CMD ["python", "worker.py"]
