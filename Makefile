1: ProducerConsumerProblem.c ConcurrentMergeSort.c QueueAtPollingBooth.c
	gcc -o PrCo ProducerConsumerProblem.c  -pthread
	gcc -o MergeSort ConcurrentMergeSort.c -pthread
	gcc -o Polling QueueAtPollingBooth.c -pthread
