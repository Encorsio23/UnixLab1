FROM alpine:3.18
RUN apk add --no-cache bash util-linux
WORKDIR /app
COPY worker.sh /app/
RUN chmod +x /app/worker.sh
VOLUME /shared
CMD ["/app/worker.sh"]
