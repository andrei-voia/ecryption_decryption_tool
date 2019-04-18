#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define N 1000000
#define MODULE_VALUE 125


char globalEncrypted[N];
char globalDecrypted[N/200];


int setRandom(int lower, int upper)
{
    //set a random number between 10 and 99
    return lower + rand() % (upper - lower);
}


int setRandomNumber()
{
    srand(time(0));
    int randomNumber = 0;
    randomNumber = setRandom(setRandom(10, 50), setRandom(60, 99));
    if(randomNumber % 10 == 0)
    {
        randomNumber += setRandom(1,9);
    }
    return randomNumber;
}


void buildCharacter(unsigned char *localCharacter, unsigned char *character)
{
    int i;
    for(i=0; i<strlen(character); i++)
    {
        localCharacter[i]=character[i];
    }
    localCharacter[strlen(character)] = NULL;
}


void shiftLeft(int n, unsigned char *localCharacter)
{
    unsigned char referenceCharacter[N];
    buildCharacter(referenceCharacter, localCharacter);

    int i;
    for(i=0; i<strlen(localCharacter); i++)
    {
        if(i+n < strlen(localCharacter))
            localCharacter[i] = referenceCharacter[i+n];
        else
            localCharacter[i] = referenceCharacter[i+n-strlen(localCharacter)];
    }
}


void reverse(unsigned char *localCharacter)
{
    unsigned char referenceCharacter[N];
    buildCharacter(referenceCharacter, localCharacter);

    int i;
    for(i=0; i<strlen(localCharacter); i++)
    {
        localCharacter[i] = referenceCharacter[strlen(referenceCharacter)-1 - i];
    }
}


void incrementASCII(int n, unsigned char *localCharacter)
{
    int i;
    for(i=0; i<strlen(localCharacter); i++)
    {
        localCharacter[i] = (int)localCharacter[i] + n;
    }
}


void scatter(int distance, int offset, unsigned char *localCharacter)
{
    int i;
    for(i=distance; i<strlen(localCharacter); i=i+distance+offset)
    {
        unsigned char aux = localCharacter[i];
        localCharacter[i] = localCharacter[i-distance];
        localCharacter[i-distance] = aux;
    }
}


void include(unsigned char *localCharacter, unsigned char character, int position)
{
    int i;
    int size = strlen(localCharacter);

    localCharacter[size + 1] = NULL;
    for(i=size; i>position; i--)
    {
        localCharacter[i] = localCharacter[i-1];
    }

    localCharacter[position] = character;
}


//add here the RANDOM number
void doXOR(unsigned char *localCharacter, unsigned char *key, int random)
{
    int value = 0;
    int countWords = random + random%10;

    int i;
    for(i=0; i<strlen(key); i++) value += key[i];

    while(value >= random*3 + value%10) value /= 2;

    //printf("\n\nBefore: %s\n", localCharacter);

    for(i=0; i<strlen(localCharacter); i++)
    {
        //printf("before: %c ",localCharacter[i]);

        countWords++;
        int module = MODULE_VALUE;   //125 works
        int count = 0;

        localCharacter[i] = localCharacter[i] ^ ((countWords + value) % module);
        int localValue = localCharacter[i];

        while(localValue < 34 || localValue > 125)  //exclude 126 because we will have problem with randomizedASCII
        {
            localCharacter[i] = localCharacter[i] ^ ((countWords + value) % module);
            module -=5;
            count ++;

            localCharacter[i] = localCharacter[i] ^ ((countWords + value) % module);
            localValue = localCharacter[i];
        }


        if(module!=MODULE_VALUE)
        {
            int j;
            for(j=0; j<count; j++)
            {
                include(localCharacter, 33, i);
                i++;
            }
            //printf("included here: ");
        }

        //printf("module: %d count: %d ", module, count);
        //printf("%c %d\n",localCharacter[i], localCharacter[i]);
    }
}


void includeRandom(unsigned char *localCharacter, unsigned char *key, int random)
{
    int a;
    int  mode = 1;
    int value = (int)key[0] / 10;

    int i;
    for(i=1; i<strlen(key); i++)
    {
        if(mode==1) value*= (int)key[i] / 10;
        else value -= (int)key[i] / 10;

        mode = -1 * mode;
    }

    a = value % 10;    //or just %(random/10)
    unsigned char first = 30 + 4* (random / 10 + a);  //first digit
    unsigned char second = 120 - 4* (random % 10 + a); //second digit

    //printf("\nfirst: %d second: %d ",first, second);

    include(localCharacter, second, 0);
    include(localCharacter, first, strlen(localCharacter));
}


int getLongest(unsigned char locationCharacter[])
{
    int count = 1;
    int longest = 0;
    int i;
    for(i=1; i<strlen(locationCharacter); i++)
    {
        if(locationCharacter[i] == locationCharacter[i-1])count++;
        else count = 1;

        if(count > longest) longest = count;
    }

    return longest;
}


void addRandomizedASCII(unsigned char *localCharacter)
{
    int length = getLongest(localCharacter) + 10;
    include(localCharacter, 33+length, 0);

    int i;
    for(i=0; i<strlen(localCharacter); i++)
    {
        //printf("before: %c %d ",localCharacter[i], localCharacter[i]);
        if((int)localCharacter[i] +1+ i%length <= 126)
            localCharacter[i] = (int)localCharacter[i] +1+ i%length;

        else
        {
            //printf(" (else root) ");
            localCharacter[i] =((int)localCharacter[i] +1+ i%length) - 126 + 33;
        }
        //printf("after %c %d  i: %d\n",localCharacter[i], localCharacter[i], 1+i%20);
    }
}


void setElement(int *order, int rest, int i)
{
    if(order[rest] == 0)order[rest]= i;

    else if(order[1] == 0)order[1]= i;
    else if(order[2] == 0)order[2]= i;
    else if(order[3] == 0)order[3]= i;
    else if(order[4] == 0)order[4]= i;
}


void setOrder(int *order, unsigned char *key)   //0, 1, 2 and 3
{
    order[1] = 0;
    order[2] = 0;
    order[3] = 0;
    order[4] = 0;

    if(strlen(key)<3)
    {
        order[1] = 1;   //shiftLeft
        order[2] = 2;   //reverse
        order[3] = 3;   //incrementASCII
        order[4] = 4;   //scatter
        return;
    }

    int space = strlen(key) / 3;
    int i;
    for(i=1; i<=3; i++)
    {
        unsigned char a = key[i*space-1];
        int rest = (int)a%4 + 1;
        //printf("%d  ",rest);
        setElement(order, rest, i);
    }
    setElement(order, 1, 4);
}


void buildMatrix(unsigned char *localCharacter, char *key, int random)
{
    int i,j;
    int offset = 1+(random%10)/3;
    int start = 1+(random/10)*2;
    int n = strlen(localCharacter);
    int len = 0;

    //setting the width
    for(i=0; i<strlen(key); i++)len+=(int)key[i];

    if(random < 40) len = len % random;
    else len = len % random / 3;

    int m = 10 + len;

    if(start>=m)start=(random/10);
    if(start>=m)start = 1;

    unsigned char matrix[m][n];


    for(j=0; j<n; j++)
        for(i=0; i<m; i++)
        {
            matrix[i][j] = NULL;
        }

    for(j=0; j<n; j++)
    {
        matrix[start][j] = localCharacter[j];
        int next = rand()%m;

        int charOffset = next - start;

        if(j!=n-1)
        matrix[(start+offset)%m][j] = 'N' + charOffset;
        start = next;

        int i;
        for(i=0; i<m; i++)
        {
            if(matrix[i][j]==NULL) matrix[i][j] = 33+rand()%94;
        }
    }

    int count = 0;
    for(j=0; j<n; j++)
    {
        for(i=0; i<m; i++)
        {
            localCharacter[count] = matrix[i][j];
            count++;
        }
    }
}


int getPow(int a, int power)
{
    int i;
    if(power==0)return 1;
    int sum = 1;

    for(i=1;i<=power;i++)
        sum = sum*a;

    return sum;
}


void converToBin(unsigned char *localCharacter)
{
    int count = 0;
    unsigned char binaryCharacter[N];

    int i,j;
    for(i=0; i<strlen(localCharacter); i++)
    {
        int bin[8];
        int countBin = 8;
        for(j=0; j<8; j++) bin[j] = 0;

        unsigned char n = localCharacter[i];
        //printf("%c: ",n);

        //save in bin a character transformed in binary
        while (n)
        {
            countBin--;
            if (n & 1)
                bin[countBin]=1;
            else
                bin[countBin]=0;
            n >>= 1;
        }
        //save in bin a character transformed in binary
        while(countBin!=0)
        {
            countBin--;
            bin[countBin]=0;
        }

        //print the binary
        //for(j=0; j<8; j++)printf("%d",bin[j]);
        //printf("\n");

        //save it in binaryCharacter
        for(j=0; j<8; j++)
        {
            binaryCharacter[count] = bin[j];
            count++;
        }

        //interchange
        for(j=0;j<=3;j++)
        {
            int aux = bin[j];
            bin[j] = bin[7-j];
            bin[7-j] = aux;
        }

        int dec = 0;
        for(j=0; j<8; j++)
        {
            if(bin[j]==1)
            dec += getPow(2,j);
        }
        //printf(" / dec: %d\n",dec);
    }

    for(i=0;i<count;i++)
    {
        localCharacter[i] = 48 + binaryCharacter[i];
    }
}


void scatterBits(unsigned char *localCharacter, unsigned char *key)
{
    int i;
    int coordinate;
    int sum = 0;
    for(i=0;i<strlen(key);i++)
        if((int)key[i]%2==0)
            sum += key[i];

    coordinate = sum/strlen(key);
    if(coordinate<1)coordinate=1;

    while(coordinate<strlen(localCharacter))
    {
        for(i=0;i<strlen(key);i++)
        {
            //printf("modify coord: %d i: %d\n",coordinate,i);
            //printf("%c ",localCharacter[coordinate+i]);

            if(localCharacter[coordinate+i] == '0') localCharacter[coordinate+i] = '1';
            else localCharacter[coordinate+i] = '0';

            //printf("-> %c\n ",localCharacter[coordinate+i]);
        }
        coordinate+=coordinate;
    }
}


void interchangeBits(unsigned char *localCharacter, unsigned char *key)
{
    int i;
    int offset = 1 + strlen(localCharacter) % strlen(key);
    int advance = 1 + strlen(key);

    //printf("\noffset: %d advance: %d\n",offset, advance);

    for(i=offset; i<strlen(localCharacter); i=i+advance)
    {
        if(i+offset>=strlen(localCharacter))return;
        unsigned char aux = localCharacter[i];
        localCharacter[i] = localCharacter[i+offset];
        localCharacter[i+offset] = aux;
    }
}


void encrypt(unsigned char *character, unsigned char *key)
{
    printf("///=============================================================================================///\n\n");
    //output purposes
    printf("Original key: %s\n", key);
    printf("Original text:\n %s\n", character);
    printf("(size %d)\n", strlen(character));   //poate &character
    //initialize
    int randomNumber = 0;
    int choice = 0;
    unsigned char localCharacter[N];
    int order[5];
    //set the random number
    randomNumber = setRandomNumber();
    //build the localCharacter
    buildCharacter(localCharacter, character);
    //sets the random order determined by the key in which the first operations are to be done
    setOrder(&order, key);
    //choose the order of the first 4 methods
    int i;
    for(i=1; i<=4; i++)
    {
        //shift the entire word right
        if(order[i] == 1)       shiftLeft(strlen(key), localCharacter);
        //reverse the word HERE
        else if(order[i] == 2)  reverse(localCharacter);
        //increment the ASCII code
        else if(order[i] == 3)  incrementASCII(strlen(key), localCharacter);
        //scatter the characters
        else if(order[i] == 4)  scatter(2+strlen(key)%5 - strlen(key)/10, 1+strlen(key)%3, localCharacter);
    }
    //do a XOR on the characters
    doXOR(localCharacter, key, randomNumber);
    //shuffles the characters so that there won't be 2 consecutive equal characters
    addRandomizedASCII(localCharacter);
    //build the matrix version
    buildMatrix(localCharacter, key, randomNumber);
    //include the random numbers in the string
    includeRandom(localCharacter, key, randomNumber);
    //convert the decimal numbers from ASCII to bin
    converToBin(localCharacter);
    //choose the order of the last 2 methods
    for(i=0; i<strlen(key); i++)
        if(key[i]%2==1) choice += key[i];

    if(choice%2==0)
    {
        //scatter the bits
        scatterBits(localCharacter, key);
        //change the bits between them
        interchangeBits(localCharacter, key);
    }
    else
    {
        //change the bits between them
        interchangeBits(localCharacter, key);
        //scatter the bits
        scatterBits(localCharacter, key);
    }


    for(i = 0; i <= strlen(globalEncrypted) + 1; i++)
    {
        globalEncrypted[i] = NULL;
    }


    //strset(globalEncrypted,NULL);
    //strcpy(globalEncrypted, localCharacter);


    for(i=0; i<strlen(localCharacter); i++)
    {
        globalEncrypted[i] = localCharacter[i];
    }
    globalEncrypted[strlen(globalEncrypted)] = NULL;


    //print the final encrypted so far / output purposes
    printf("\n\nEncrypted:\n%s\n", globalEncrypted);
    printf("(size %d)", strlen(globalEncrypted));
    //for test purposes only
    decrypt(globalEncrypted, key);
}






//TO DO FOR DECRYPTION
void shiftRight(int n, unsigned char *localCharacter)
{
    unsigned char referenceCharacter[N];
    buildCharacter(referenceCharacter, localCharacter);

    int i;
    for(i=0; i<strlen(localCharacter); i++)
    {
        if(i-n >= 0)
            localCharacter[i] = referenceCharacter[i-n];
        else
            localCharacter[i] = referenceCharacter[strlen(localCharacter) +i-n];
    }
}


void deleteIncluded(unsigned char *localCharacter, int position)
{
    int i;
    for(i=position; i<=strlen(localCharacter)-1; i++)
    {
        localCharacter[i] = localCharacter[i+1];
    }
}


//add here the RANDOM number
void reverseXOR(unsigned char *localCharacter, unsigned char *key, int random)
{
    //printf("\n\n\n\nSTART: %s\n",localCharacter);
    //printf("size %d\n\n",strlen(localCharacter));

    int value = 0;
    int countWords = random + random%10;

    //calculate stuff
    int i;
    for(i=0; i<strlen(key); i++) value += key[i];
    while(value >= random*3 + value%10) value /= 2;

    int count = 0;

    for(i=0; i<strlen(localCharacter); i++)
    {
        int module = MODULE_VALUE;   //125 works
        if(localCharacter[i]=='!')
        {
            //deleteIncluded(localCharacter, i);
            count++;
            //printf("deleted + SIZE: %d",strlen(localCharacter));
            //printf("deleted");

            int j;
            for(j=i; j<strlen(localCharacter); j++)
            {
                localCharacter[j] = localCharacter[j+1];
            }

            i--;
        }

        //this  happens when you have actual encrypted code
        else
        {
            countWords ++;
            //set the module
            module = module - (5*count);

            //printf("before: %c module: %d ",localCharacter[i], module);

            localCharacter[i] = localCharacter[i] ^ ((countWords + value) % module);

            count = 0;

            //printf("%c %d\n",localCharacter[i], localCharacter[i]);
        }
    }
}


int getRandom(unsigned char *localCharacter, unsigned char *key)
{
    int random;
    int randomFirst, randomSecond;
    int second = localCharacter[0];
    int first  = localCharacter[strlen(localCharacter)-1];

    deleteIncluded(localCharacter, 0);
    deleteIncluded(localCharacter, strlen(localCharacter)-1);

    int a;
    int  mode = 1;
    int value = (int)key[0] / 10;

    int i;
    for(i=1; i<strlen(key); i++)
    {
        if(mode==1) value*= (int)key[i] / 10;
        else value -= (int)key[i] / 10;

        mode = -1 * mode;
    }

    a = value % 10;
    randomFirst = ((first - 30)/4 -a);
    randomSecond = (second - 120)/(-4) - a;

    random = randomFirst * 10 + randomSecond;
    return random;
}


void extractRandomizedASCII(unsigned char *localCharacter)
{
    int length = 0;
    int i;
    for(i=0; i<strlen(localCharacter); i++)
    {
        if(i==0)
        {
            length = localCharacter[i] -34;
            continue;
        }
        //printf("before: %c %d ",localCharacter[i], localCharacter[i]);
        if((int)localCharacter[i] -1- i%length >= 33)
            localCharacter[i] = (int)localCharacter[i] -1- i%length;

        else
        {
            //printf(" (else root) ");
            int  x;
            int y;
            x = localCharacter[i] - 33;
            y = 126 + x;
            localCharacter[i] = y -1- i%length;
        }
        //printf("after: %c %d  i: %d\n",localCharacter[i], localCharacter[i], 1+i%20);
    }
    deleteIncluded(localCharacter, 0);
}


void converToDecimal(unsigned char *localCharacter)
{
    int count = 0;
    int countBinary = 0;
    int i,j;
    unsigned char binaryCharacter[N];

    for(i=0; i<strlen(localCharacter);i=i+8)
    {
        int bin[8];
        int count = 7;

        for(j=i;j<=i+7;j++)
        {
            bin[count]=(int)localCharacter[j]-48;
            count --;
        }

        //make it decimal
        int dec = 0;
        for(j=0; j<8; j++)
        {
            if(bin[j]==1)
            dec += getPow(2,j);
        }

        //save it in decimal string
        binaryCharacter[countBinary]=dec;
        countBinary++;
        binaryCharacter[countBinary]=NULL;
    }
    //reset the original string
    int size = strlen(localCharacter);
    for(i=0;i<=size;i++)localCharacter[i]=NULL;
    //copy in original string
    for(i=0;i<strlen(binaryCharacter);i++)
    {
        localCharacter[i]=binaryCharacter[i];
    }
}


void decryptMatrix(unsigned char *localCharacter, char *key, int random)
{
    int i,j;
    int offset = 1+(random%10)/3;
    int start = 1+(random/10)*2;
    int len = 0;

    //setting the width
    for(i=0; i<strlen(key); i++)len+=(int)key[i];

    if(random < 40) len = len % random;
    else len = len % random / 3;

    int m = 10 + len;
    int n = strlen(localCharacter)/m;

    if(start>=m)start=(random/10);
    if(start>=m)start = 1;

    unsigned char matrix[m][n];
    for(j=0; j<n; j++)
    {
        for(i=0; i<m; i++)
        {
            matrix[i][j] = NULL;
        }
    }

    int count = 0;
    for(j=0; j<n; j++)
    {
        for(i=0; i<m; i++)
        {
            matrix[i][j] = localCharacter[count];
            count++;
        }
    }

    //reset pointer
    for(i=0;i<m*n;i++)localCharacter[i] = NULL;


    //rebuild pointer
    for(j=0; j<n; j++)
    {
        localCharacter[j] = matrix[start][j];
        start = start + (int)(matrix[(start+offset)%m][j]-'N');
    }
}


void decrypt(unsigned char character[], unsigned char *key)
{
    //initialized stuff
    int random = 0;
    int order[5];
    int choice = 0;

    //choose the order of the last 2 methods
    int i;
    for(i=0; i<strlen(key); i++)
        if(key[i]%2==1) choice += key[i];

    if(choice%2==0)
    {
        interchangeBits(character, key);
        scatterBits(character,key);
    }
    else
    {
        scatterBits(character,key);
        interchangeBits(character, key);
    }

    converToDecimal(character);
    //gets the random value  and also removes it from the string
    random = getRandom(character, key);     //this also deletes the random characters
    //decrypts the matrix and creates a string
    decryptMatrix(character, key, random);
    //extracts all randomizedASCIIs
    extractRandomizedASCII(character);
    //reverses the XOR
    reverseXOR(character, key, random);
    //do the 4 operations in their random order established with the key
    setOrder(&order, key);
    for(i=4; i>=1; i--)
    {
        if(order[i] == 1)       shiftRight(strlen(key), character);
        else if(order[i] == 2)  reverse(character);
        else if(order[i] == 3)  incrementASCII(-1*strlen(key),character);   //reverse of the incrementation (decrement)
        else if(order[i] == 4)  scatter(2+strlen(key)%5 - strlen(key)/10, 1+strlen(key)%3, character);
    }


    //strset(globalDecrypted, NULL);
    //strcpy(globalDecrypted, character);

    for(i=0; i<strlen(character); i++)
    {
        globalDecrypted[i] = character[i];
    }
    globalDecrypted[strlen(globalDecrypted)] = NULL;


    //output purposes
    printf("\n\nDecrypted:\n %s\n", globalDecrypted);
    printf("(size %d)\n\n",strlen(globalDecrypted));
    printf("///================================================================================================================///\n");
}


void basicEncrypt()
{

}


int main()
{
    //encrypt("Mama spune:\n-Eu nu am zburat cu avionul niciodata si nu voi zbura niciodata pentru ca mi-e frica.", "code");
    //encrypt("Ciocolata", "simple");
    //encrypt("Ciocolata e foarte buna pentru ca e alba", "cod");
    //encrypt("Zodia mea e jmek", "smiley");
    //encrypt("Ce ar fi sa scriu un text cu scopul de a testa un mesaj mai lung daca acesta se poate cripta sau nu", "testing");
    //encrypt("Rada are pere multe si mari, chiar foarte mari, incredibil de mari.\nAstfel poate sa duca multe bagaje grele pentru Mung-Daal.", "simple");
    encrypt("Did I ever tell you what the definition of insanity is? Insanity is doing the exact... same fucking thing... over and over again expecting... shit to change... That. Is. Crazy. The first time somebody told me that, I dunno, I thought they were bullshitting me, so, boom, I shot him. The thing is... he was right. And then I started seeing, everywhere I looked, everywhere I looked all these fucking pricks, everywhere I looked, doing the exact same fucking thing... over and over and over and over again thinking: \"This time is gonna be different.\" No, no, no please... This time is gonna be different.", "vaas");

    char inputText[2000];
    char key[100];

    //input text / stuff
    printf("Input text: ");
    fgets(inputText, 100, stdin);
    inputText[strlen(inputText) - 1] = NULL;
    //input key
    printf("Input key: ");
    scanf("%s", &key);

    printf("\n%s\n",inputText);
    printf("%s",key);




    return 0;
}
