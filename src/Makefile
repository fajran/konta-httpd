OPT=-DFORKNEWPROCESS
OBJECTS=konta-httpd.o readdir.o util.o config.o request-handler.o cgi.o response.o
BIN=konta-httpd

all : $(BIN)

$(BIN) : $(OBJECTS)
	@gcc $(OPT) -o $@ $? 
	@echo "  [LINK]   $@"

konta-httpd.o : konta-httpd.c
	@gcc $(OPT) -c $?
	@echo "  [CC]     $@"

readdir.o : readdir.c
	@gcc $(OPT) -c $?
	@echo "  [CC]     $@"

util.o : util.c
	@gcc $(OPT) -c $?
	@echo "  [CC]     $@"

config.o : config.c
	@gcc $(OPT) -c $?
	@echo "  [CC]     $@"

request-handler.o : request-handler.c
	@gcc $(OPT) -c $?
	@echo "  [CC]     $@"

cgi.o : cgi.c
	@gcc $(OPT) -c $?
	@echo "  [CC]     $@"
	
response.o : response.c
	@gcc $(OPT) -c $?
	@echo "  [CC]     $@"

clean :
	@rm -f $(BIN) $(OBJECTS)
	@echo "  [RM]     $(BIN) $(OBJECTS)"

