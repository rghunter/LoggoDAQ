#ifndef circular_h
#define circular_h

//Circular buffer with attempt at atomic message write
//The buffer consists of a block of memory which conceptually contains
//two parts - data which is ready to be flushed, and data which is not
//yet ready. The buffer writer decides when the data which wasn't ready
//becomes so, and when it is, it is added to the data which is ready.
//The buffer gets written to by the generic fill() function. 
//A special drain() function empties data out of the buffer somehow and
//moves the head ptr up.
//All circular buffers are 1024 characters.
typedef struct circular {
  volatile char buf[1024];
  //Location of the next slot to be written to
  volatile int head;
  //Location of the next slot not yet ready to be flushed
  volatile int mid;
  //Location of the next slot to be flushed
  volatile int tail;
} circular;

//Is there no space to write another char?
int isFull(circular* buf);
//Is there at least one char ready to be read?
int isEmpty(circular* buf);
//Add a char to the buffer, not ready to be flushed yet
int fill(circular* buf, char in);

void empty(circular* buf);

//Built upon fill
int fillString(circular* buf, char* in);
int fillDec(circular* buf, int in);
int fill0Dec(circular* buf, int in, int len);
int fillHex(circular* buf, unsigned int in, int len);
int fillStringn(circular *buf, char* in, int len);
int fillShort(circular* buf, short in);
int fillInt(circular* buf, int in);

//Mark all current unready data as ready
void mark(circular* buf);
//Get the next character ready to be flushed
char get(circular* buf);
//Get all ready data from one buffer and copy it to another (as ready also)
int drain(circular* from, circular* to);
//Get the number of characters which aren't ready yet
int unreadylen(circular* buf);
//Get the number of characters which are ready
int readylen(circular* buf);

char peekTail(circular* buf, int ahead);
char peekMid(circular* buf, int ahead);
short peekMidShort(circular* buf, int ahead);
int peekMidInt(circular* buf, int ahead);
char peekHead(circular* buf, int ahead);

void pokeTail(circular* buf, int ahead, char poke);
void pokeMid(circular* buf, int ahead, char poke);
void pokeHead(circular* buf, int ahead, char poke);
#endif

