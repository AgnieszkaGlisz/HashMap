#include <stdio.h>
#include <stdlib.h>

typedef enum {
	false,
	true
} bool;

typedef struct Value {
	void* value;
	struct Value* next;
} Values;

typedef struct Key {
	int key;
	struct Key* next;
	Values* firstValue;
} Keys;

typedef struct {
	Keys* head;
	void*(*hashFunction)(void* value);
	bool(*compare)(void* a, void* b);
} HashMap;
// structures


void valuesInit(Values* v, void* value, Values* n) {
	v->value = value;
	v->next = n;
}

void keysInit(Keys* contener, int key, Keys* n, Values* f) {
	contener->key = key;
	contener->next = n;
	contener->firstValue = f;
}

void hashMapInit(HashMap* contener, Keys* k, void*(*hashF)(void* value), bool(*comp)(void* a, void* b)) {
	contener->head = k;
	contener->hashFunction = hashF;
	contener->compare = comp;
}

HashMap* init(void* value, int(*hashFunction)(void* val), bool(*comp)(void* a, void* b)) {
	Keys* head = malloc(sizeof(Keys));
	Values* first = malloc(sizeof(Values));
	HashMap* handle = malloc(sizeof(HashMap));

	keysInit(head, hashFunction(value), NULL, first);
	valuesInit(first, value, NULL);
	hashMapInit(handle, head, hashFunction, comp);
	return handle;
}
// init functions



void insertValue(Keys* head, void* value) {
	Values* newV = malloc(sizeof(Values));
	valuesInit(newV, value, NULL);

	if (head->firstValue == NULL) {
		head->firstValue = newV;
	}
	else {
		Values* tmp = head->firstValue;
		while (tmp->next != NULL) {
			tmp = tmp->next;
		}
		tmp->next = newV;
	}

}

void push(HashMap* handle, void* value) {
	int* key = malloc(sizeof(int));
	*key = handle->hashFunction(value);

	Keys* newKey = malloc(sizeof(Keys));
	keysInit(newKey, *key, NULL, NULL);

	Keys* tmp = handle->head;
	if (tmp == NULL) {
		handle->head = newKey;
		return;
	}
	else if (tmp->key > *key) {
		Keys* tmpK = handle->head;
		handle->head = newKey;
		newKey->next = tmpK;
		insertValue(newKey, value);
		free(key);
		return;
	}

	while (tmp->next != NULL) {
		if (tmp->key == *key) {
			insertValue(tmp, value);
			free(key);
			free(newKey);
			return;
		}
		else if ((tmp->key < *key) && (tmp->next->key > *key)) {
			Keys* tmpN = tmp->next;
			tmp->next = newKey;
			newKey->next = tmpN;
			insertValue(newKey, value);
			free(key);
			return;
		}
		tmp = tmp->next;
	}
	if (tmp->key == *key) {
		insertValue(tmp, value);
		free(key);
		free(newKey);
	}
	else {
		tmp->next = newKey;
		insertValue(newKey, value);
		free(key);
	}
}

Values* popValue(HashMap* handle, void* value, Keys* tmp) {
	Values* tmpV = tmp->firstValue;
	if (tmpV == NULL) return;
	else if (handle->compare(value, tmpV->value) == true) {
		tmp->firstValue = tmpV->next;
		return tmpV;
	}
	else {
		Values* tmpVN = tmpV->next;
		while (tmpVN != NULL) {
			if (handle->compare(tmpVN->value, value) == true) {
				tmpV->next = tmpVN->next;
				return tmpVN;
			}
			tmpV = tmpV->next;
			tmpVN = tmpVN->next;
		}

	}
}

void freeValue(HashMap* handle, void* value) {
	int* key = malloc(sizeof(int));
	*key = handle->hashFunction(value);
	Keys* tmp = handle->head;
	while (tmp != NULL) {
		if (tmp->key == *key) {
			Values* val = popValue(handle, value, tmp);
			free(val->value);
			free(val);
			return;
		}
		tmp = tmp->next;
	}
}

void freeValues(Keys* head) {
	Values* tmpPrev = head->firstValue;
	if (tmpPrev == NULL) return;
	Values* tmpNext = tmpPrev->next;
	free(tmpPrev->value);
	free(tmpPrev);
	while (tmpNext != NULL) {
		tmpPrev = tmpNext;
		tmpNext = tmpNext->next;
		free(tmpPrev->value);
		free(tmpPrev);
	}
}

void freeMem(HashMap* handle) {
	Keys* tmpPrev = handle->head;
	if (tmpPrev == NULL) { 
		free(handle); 
		return;
	}
	Keys* tmpNext = tmpPrev->next;
	freeValues(tmpPrev);
	free(tmpPrev);
	while (tmpNext != NULL) {
		tmpPrev = tmpNext;
		tmpNext = tmpNext->next;
		freeValues(tmpPrev);
		free(tmpPrev);
	}
	free(handle);
}

void display(Keys* head, void(*displayVal)(void* k)) {
	Keys* tmp = head;
	Values* tmpV;
	while (tmp != NULL) {
		printf("   [%d] ", tmp->key);
		tmpV = tmp->firstValue;
		while (tmpV != NULL) {
			printf(" -> ");
			displayVal(tmpV->value);
			tmpV = tmpV->next;
		}
		if (tmp->next != NULL) printf("\n    | \n");
		tmp = tmp->next;
	}
	printf("\n\n\n");
}

void displayINT(void* x) {
	printf("%d", (*(int*)x));
}

int hash(void* x) {
	return (*(int*)x) % 10;
}

bool compare(void* x, void * y) {
	if (*(int*)x == *(int*)y) return true;
	return false;
}

int main() {
	int(*wskHash)(void* x);
	wskHash = hash;

	void(*wskDisp)(void* q);
	wskDisp = displayINT;

	bool(*wskCompare)(void* a, void* b);
	wskCompare = compare;

	int tab[] = { 23,45,11,89,23,67,89,26,92,54,31,90,27,52,65,57,17,78,21,66,42,97 }; //(21)
	int** x = malloc(sizeof(tab) / sizeof(int) * sizeof(int*));
	for (int i = 0; i < sizeof(tab) / sizeof(int); i++) {
		x[i] = malloc(sizeof(int));
		x[i][0] = tab[i];
	}


	printf("1._____________________________________\nInicjalizacja:\n");
	HashMap* h = init(&x[0][0], wskHash, wskCompare);
	for (int i = 1; i < sizeof(tab) / sizeof(int); i++) {
		push(h, &x[i][0]);
	}

	display(h->head, wskDisp);

	printf("1._____________________________________\nUsuniecie wartosci(17):\n");
	freeValue(h, &x[16][0]);
	display(h->head, wskDisp);

	printf("1._____________________________________\nUsuniecie wartosci(97):\n");
	freeValue(h, &x[21][0]);
	display(h->head, wskDisp);

	printf("1._____________________________________\nUsuniecie wartosci(23):\n");
	freeValue(h, &x[0][0]);
	display(h->head, wskDisp);

	printf("1._____________________________________\nUsuniecie wartosci(11):\n");
	freeValue(h, &x[2][0]);
	display(h->head, wskDisp);

	freeMem(h);

	free(x);
	return 0;
}