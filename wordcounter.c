// Joel Rytkönen 2544166

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

#define MAXCHAR 400                 // yksittäisen rivin maksimipituus
#define HASH_TABLE_SIZE 49999       // hash-taulukon koko
#define WORD_LENGTH 25              // sanan maksimipituus

// Muuttujatyyppi totuusarvoisille muuttujille
typedef enum {false, true} bool;

// Hash-taulukon muuttujatyyppi
struct words {
    char word[WORD_LENGTH];
    int count;
};

// FUNKTIOT

void splitStr(char *str, struct words table[], int *total_words, int *different_words);
int wordKey(char *word);
int hash(int number);
int search_word(int number, char *word, struct words table[]);
int insert_word(int number, char *word, struct words table[]);
void heapsort(struct words table[], int heap_size);
void build_max_heap(struct words table[], int heap_size);
void max_heapify(struct words table[], int index, int heap_size);


int main()
{
    FILE *fp;
    char str[MAXCHAR];
    char filename[30];
    // Tekstitiedoston nimi syötteenä
    printf("Name of the text file: ");
    scanf("%s", filename);
    printf("\n");
    
    // Ohjelman aloitusaika
    time_t start_time = time(NULL);
    
    // Käytettävä hash-taulukko
    struct words hashtable[HASH_TABLE_SIZE];
    
    // Sanalaskurit
    int total_words = 0;
    int different_words = 0;
    
    // Taulukon alustus
    for(int i=0; i < HASH_TABLE_SIZE; i++)
    {
        strcpy(hashtable[i].word, "0");
        hashtable[i].count = 0;
    }
    
    // Avataan tekstitiedosto
    fp = fopen(filename, "r");
    if (fp == NULL)
    {
        printf("Could not open file %s",filename);
        return 1;
    }
    
    // Sanojen erittely
    while (fgets(str, MAXCHAR, fp) != NULL)
    {
        // Poistetaan \n-merkki rivin lopusta
        if (str[strlen(str) - 1] == '\n')
        {
            str[strlen(str) - 1] = '\0';
        }
        splitStr(str, hashtable, &total_words, &different_words);
    }
    
    // Suljetaan tekstitiedosto
    fclose(fp);
    
    // Lajitellaan hash-taulukko maksimikeon avulla
    heapsort(hashtable, HASH_TABLE_SIZE);
    
    // Tulostukset
    printf("Total number of words = %d\n", total_words);
    printf("Number of different words = %d\n", different_words);
        
    printf("The 100 most common words:\n");
    printf("%-20s NUMBER OF OCCURRENCES\n", "WORD");
    
    int lastIndex = HASH_TABLE_SIZE - 1;
    
    for (int i = lastIndex; i > lastIndex - 100; i--)
    {
        if (strcmp(hashtable[i].word, "0") != 0)
        {
            printf("%-20s %d\n", hashtable[i].word, hashtable[i].count);
        }
    }
    
    // Ohjelman lopetusaika
    time_t end_time = time(NULL);
    // Ohjelman kesto
    time_t run_time = end_time - start_time;
    printf("Run time: %I64d seconds\n", run_time);
    
    return 0;
    
}

// Erittelee sanat tekstirivistä ja tallentaa ne hash-taulukkoon
void splitStr(char *str, struct words table[], int *total_words, int *different_words)
{
    
    bool new_word = false;      // Merkkaa uuden sanan alkua
    int length = strlen(str);
    char *word;
    int word_start_index;
    int word_end_index;
    int table_index;
    int key;
    
    for (int i = 0; i <= length; i++)
    {
        // Etsitään ainoastaan kirjaimia tai heittomerkkiä
        // Uuden sanan alkaessa otetaan indeksi talteen
        if (isalpha(str[i]) != 0 || str[i] == 39)       // Heittomerkin ASCII-koodi 39
        {
            if (new_word == false)
            {
                new_word = true;
                word_start_index = i;
            }
        }
        // Jos joku muu merkki ja sana kesken, sana loppuu
        else
        {
            if (new_word == true)
            {
                new_word = false;
                word_end_index = i - 1;     // Sanan loppuindeksi talteen
                int k = 0;
                
                // Sana word-muuttujaan
                for (int j = word_start_index; j <= word_end_index; j++)
                {
                    word[k] = str[j];
                    k++;
                }
                word[k] = '\0';
                
                word = strlwr(word);
                key = wordKey(word);
                
                // Tarkastetaan onko sana ollut tekstissä aiemmin
                // Jos on kasvatetaan sanan laskuria
                // Jos ei lisätään sanan avain hash-taulukkoon
                if ((table_index = search_word(key, word, table)) != -1)
                {
                    table[table_index].count++;
                }
                else
                {
                    if(insert_word(key, word, table) < 0)
                    {
                        printf("Hash-table full\n");
                    }
                    (*different_words)++;
                }
                
                (*total_words)++;
            }
        }
    }
    
}


// Muuttaa sanat numeroiksi
int wordKey(char *word)
{
    
    int length = strlen(word);
    int i, number = 0;
    
    // Summataan sanan kirjaimia vastaavat ASCII-koodit yhteen
    for (i = 0; i < length; i++)
    {
        number = number + word[i];
    }
    
    return number;
    
}

// Tiivistefunktio
int hash(int number)
{
    return number%HASH_TABLE_SIZE;
}


// Lisää sanan hash-taulukkoon käyttäen lineaarista luotausta
// ja alustaa sanan laskurin ykköseksi
// Palauttaa -1 jos taulukko täysi
int insert_word(int number, char *word, struct words table[])
{
    
    int f = hash(number);
    int i;
    int g;
    for (i=0; i<=HASH_TABLE_SIZE; i++)
    {
        g = f + i;
        int index = hash(g);
        if (strcmp(table[index].word, "0") == 0)
        {
            strcpy(table[index].word, word);
            table[index].count = 1;
            return 0;
        }
    }
    return -1;
    
}

// Etsii sanan hash-taulukosta
// Palauttaa indeksin jos sana löytyy ja -1 jos ei löydy
int search_word(int number, char *word, struct words table[])
{
    
    int i = 0;
    int f = hash(number);
    int g = f + i;
    int j = hash(g);
    while (i<HASH_TABLE_SIZE && strcmp(table[j].word, "0") != 0)
    {
        if (strcmp(table[j].word, word) == 0)
        {
            return j;
        }
        i++;
        
        if (i<HASH_TABLE_SIZE)
        {
            g = f + i;
            j = hash(g);
        }
    }
    return -1;

}

// Lajittelee maksimikeon alkiot pienimmästä suurimpaan
void heapsort(struct words table[], int heap_size)
{
    
    build_max_heap(table, heap_size);
    struct words x;
    int heap_length = heap_size - 1;
    for (int i = heap_length; i >= 1; i--)
    {
        x = table[0];
        table[0] = table[i];
        table[i] = x;
        heap_size = heap_size - 1;
        max_heapify(table, 0, heap_size);
    }
    
}

// Rakentaa maksimikeon
void build_max_heap(struct words table[], int heap_size)
{
    
    // Viimeisen ei-lehtisolmun indeksi
    int startIndex = (heap_size / 2) - 1;
    
    for (int i = startIndex; i >= 0; i--)
    {
        max_heapify(table, i, heap_size);
    }
    
}

// Lajittelee alipuun, jonka juurena index, maksimikeoksi
void max_heapify(struct words table[], int index, int heap_size)
{
    
    int left = 2 * index + 1;       // Solmun vasen lapsi
    int right = 2 * index + 2;      // Solmun oikea lapsi
    int largest;
    struct words x;
    
    // Onko vasen lapsi isompi kuin juuri
    if (left < heap_size && table[left].count > table[index].count)
    {
        largest = left;
    }
    else
    {
        largest = index;
    }
    
    // Onko oikea lapsi isompi kuin juuri
    if (right < heap_size && table[right].count > table[largest].count)
    {
        largest = right;
    }
    
    if (largest != index)
    {
        x = table[index];
        table[index] = table[largest];
        table[largest] = x;
        // Tehdään lajittelu muuttuneelle alipuulle
        max_heapify(table, largest, heap_size);
    }
    
}