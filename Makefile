NAME = uart_study

FBT = ./fbt

TMPNAME = template
ifndef NAME
	NAME = new_program
endif


all: run

run:
	@cd ../.. && \
		$(FBT) launch_app APPSRC=applications/$(NAME)

rename:
	@mv $(TMPNAME).c $(NAME).c
	@sed -i 's/$(TMPNAME)_/$(NAME)_/g' $(NAME).c
	@sed -i 's/$(TMPNAME).c/$(NAME).c/g' $(NAME).c
	@OLD=$(shell echo $(TMPNAME) | sed -r 's/(.)[_ ]+(.)/\1\U\2/g;s/^[a-z]/\U&/') && \
	 NEW=$(shell echo $(NAME)    | sed -r 's/(.)[_ ]+(.)/\1\U\2/g;s/^[a-z]/\U&/') && \
		sed -i "s/$${OLD}State/$${NEW}State/g" $(NAME).c
	@sed -i 's/$(TMPNAME)/$(NAME)/g' application.fam
	@OLD=$(shell echo $(TMPNAME) | tr '[:lower:]' '[:upper:]') && \
	 NEW=$(shell echo $(NAME)    | tr '[:lower:]' '[:upper:]') && \
		sed -i "s/$$OLD/$$NEW/g" application.fam
