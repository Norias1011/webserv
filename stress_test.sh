#!/bin/bash

URL="http://localhost:8002"
NUM_REQUESTS=1000
CONCURRENCY=10

# Function to send requests
send_requests() {
    for i in $(seq 1 $NUM_REQUESTS); do
        curl -s -o /dev/null -w "%{http_code}\n" $URL &
    done
    wait
}

# Run the function with specified concurrency
for i in $(seq 1 $CONCURRENCY); do
    send_requests &
done

wait