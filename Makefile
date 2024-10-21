thready: thready.c
	gcc -o thready thready.c

results: thready
	(echo "Pattern 1:"; ./thready 1 $$((RANDOM % 15 + 5)); \
	echo; \
	echo "Pattern 2:"; ./thready 2 $$((RANDOM % 15 + 5)); \
	echo; \
	echo "Pattern 3:"; ./thready 3 $$((RANDOM % 20 + 5))) \
		> results.txt 2>&1

clean:
	rm -f thread
