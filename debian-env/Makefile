USER = $(shell whoami)
PARENT_PATH = $(shell dirname $(shell pwd))
PARENT_NAME = $(shell basename $(PARENT_PATH))
IMAGE = $(USER)-debian-env-image
CONTAINER = $(USER)-debian-env-container

all: db dr

db:
	@docker build -t $(IMAGE) .

dr:
	@docker run -it --rm -v $(PARENT_PATH):/debian-env-$(PARENT_NAME) -w /debian-env-$(PARENT_NAME) --privileged --name $(CONTAINER) $(IMAGE) zsh

bcon:
	@sysctl net.ipv4.icmp_echo_ignore_broadcasts=0

bcoff:
	@sysctl net.ipv4.icmp_echo_ignore_broadcasts=1

.PHONY: all db dr bcon bcoff