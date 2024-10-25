#!/bin/sh

print_usage() {
  echo "Usage: $0 <command> <timeout>"
  echo "  <command> : Command to execute"
  echo "  <timeout> : Timeout in seconds"
  echo ""
  echo "Example: $0 'ls /some_directory' 120"
}

if [ $# -ne 2 ]; then
  print_usage
  exit 1
fi

command="$1"
timeout="$2"
start_time=$(date +%s)

while true; do
  # Execute the command
  eval "$command"
  if [ $? -eq 0 ]; then
    echo "Success"
    exit 0
  fi

  # Check if the timeout has been reached
  current_time=$(date +%s)
  elapsed_time=$((current_time - start_time))
  if [ $elapsed_time -ge "$timeout" ]; then
    echo "Fail"
    exit 1
  fi

  # Sleep for a short interval before checking again
  sleep 1
done

