PREFIX = cognisafe
IMAGE_NAME = eclipse-mosquitto
CONTAINER_NAME = $(PREFIX)-container

nyamuk-init:
	docker run -d -p 1883:1883 --name $(CONTAINER_NAME) $(IMAGE_NAME) 
nyamuk-start:
	docker start $(CONTAINER_NAME)
