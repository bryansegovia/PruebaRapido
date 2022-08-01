EXEC = check_hash
S256 = sha256
S1 = sha1
DLIB1 = lib$(S256).so
DLIB2 = lib$(S1).so

$(EXEC): $(EXEC).c $(DLIB1) $(DLIB2)
	gcc -o $@ $< ./$(DLIB1) ./$(DLIB2)

$(DLIB1): $(S256).c
	gcc -shared -fPIC -o $@ $^

$(DLIB2): $(S1).c
	gcc -shared -fPIC -o $@ $^

.PHONY: clean
clean:
	rm -rf $(EXEC) *.o $(DLIB1) $(DLIB2)