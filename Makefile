build:
	@mkdir -p ./obj/
	@gcc -Wall -g -o ./obj/main main.c

run: build
	@./obj/main

clean:
	@rm -rf ./obj/
