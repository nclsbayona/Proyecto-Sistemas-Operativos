#include <stdbool.h>
#include <stdlib.h>
typedef struct Value
{
    int len;
    char **filenames;
};

typedef struct Key
{
    char key;
};

typedef struct Entry
{
    struct Key *key;
    struct Value *value;
};

typedef struct Map
{
    int len;
    struct Entry **entries;
};

struct Entry *searchEntryInMap(struct Map *map, const char key)
{
    struct Entry *entry = NULL;
    for (int i = 0; i < map->len; i++)
        if (map->entries[i]->key->key == key)
            entry = map->entries[i];
    return entry;
}

bool addEntryToMap(struct Map *map, struct Entry *entry)
{
    bool r = true;
    if (map->len++ > 0)
        map->entries = realloc(map->entries, sizeof(struct Entry *) * map->len);
    else
        map->entries = malloc(sizeof(struct Entry *));
    map->entries[map->len - 1] = entry;
    return r;
}

struct Entry *createEntry(char key, struct Value *value)
{
    struct Entry *entry = malloc(sizeof(struct Entry));
    entry->key = malloc(sizeof(struct Key));
    entry->key->key = key;
    entry->value = value;
    return entry;
}

void subscribeToEntry(struct Map *map, char key, char *filename)
{
    struct Entry *entry = searchEntryInMap(map, key);
    if (!entry)
    {
        struct Value *value = malloc(sizeof(struct Value));
        value->len = 0;
        value->filenames = malloc(sizeof(char *) * value->len);
        entry = createEntry(key, value);
        addEntryToMap(map, entry);
    }
    else if (!entry->value){
        entry->value = malloc(sizeof(struct Value));
        entry->value->len = 0;
    }
    if (!entry->value->filenames)
        entry->value->filenames = malloc(sizeof(char *));
    else
        entry->value->filenames = realloc(entry->value->filenames, sizeof(char *) * (++entry->value->len));
    entry->value->filenames[entry->value->len - 1] = filename;
}

struct Map *createMap()
{
    struct Map *map = (struct Map *)malloc(sizeof(struct Map));
    map->len = 0;
    map->entries = NULL;
    return map;
}
/*
void printMap(struct Map *map)
{
    printf("Map\n\n");
    for (int i = 0; i < map->len; i++)
    {
        printf("%c: ", map->entries[i]->key->key);
        for (int j = 0; j < map->entries[i]->value->len; j++)
            printf("%s ", map->entries[i]->value->filenames[j]);
        printf("\n");
    }
}

int main(){
    struct Map *map = createMap();
    printMap(map);
    subscribeToEntry(map, 'a', "a");
    subscribeToEntry(map, 'a', "b");
    subscribeToEntry(map, 'b', "c");
    subscribeToEntry(map, 'b', "d");
    subscribeToEntry(map, 'c', "e");
    subscribeToEntry(map, 'c', "f");
    subscribeToEntry(map, 'c', "g");
    subscribeToEntry(map, 'd', "h");
    subscribeToEntry(map, 'd', "i");
    subscribeToEntry(map, 'd', "j");
    subscribeToEntry(map, 'e', "k");
    subscribeToEntry(map, 'e', "l");
    subscribeToEntry(map, 'e', "m");
    subscribeToEntry(map, 'f', "n");
    subscribeToEntry(map, 'f', "o");
    subscribeToEntry(map, 'f', "p");
    subscribeToEntry(map, 'g', "q");
    subscribeToEntry(map, 'g', "r");
    subscribeToEntry(map, 'g', "s");
    subscribeToEntry(map, 'h', "t");
    subscribeToEntry(map, 'h', "u");
    subscribeToEntry(map, 'h', "v");
    subscribeToEntry(map, 'i', "w");
    subscribeToEntry(map, 'i', "x");
    subscribeToEntry(map, 'i', "y");
    subscribeToEntry(map, 'j', "z");
    subscribeToEntry(map, 'j', "aa");
    subscribeToEntry(map, 'j', "bb");
    subscribeToEntry(map, 'k', "cc");
    subscribeToEntry(map, 'k', "dd");
    printMap(map);
}*/