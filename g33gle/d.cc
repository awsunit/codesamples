#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <iostream>
using std::string;
using std::cout;
using std::endl;
void get(string *str) {
  char *c = (char *)malloc(sizeof(char)*5);
  char *temp;
  for (int i = 0; i < 4; i++) {
    *temp++ = i;
  }
  *temp = '\0';
  *str = *c;
}
int main(int argc, char **argv) {
  /*
  
char c[5] = {'h', 'e', 'l', 'l'};
  std::string k = c;
  std::string *s = &k;
  std::cout << "s string is: " << *s << std::endl;
  std::cout << "k string is: " << k << std::endl;
  std::cout << "c string is: " << c << std::endl;
  */
  
  int fd = open(argv[1], O_RDONLY);
  char s[1024];
  //string s(1024, '0');
  int i = read(fd, &s, 1023);
  s[i] = '\0';
  string blow = s;
  cout << s << endl;
  
  /* unsigned char c[5] = {'h', 'e', 'l', 'l'};
  string s(reinterpret_cast<char *>(c));
  cout << s << endl;
 
  string s = "this be bad";
  size_t p = s.find(" ");
  cout << "first occurrence: " << p << endl;
  size_t q = s.find(" ", p + 1);
  cout << "next occurrence: " << q << endl;
  string t = s.substr(p+1, q-p-1);
  cout << "inbetween occurrences:" << t << "TT" << endl;
*/
  bool r = true;
  bool t = true;
  bool f = false;
  r &= t;
  cout << "r &= t: " << r << endl;
  r &= f;
  cout << "r &=f: " << r << endl; 


  cout << "all done boss" << endl;
  return 0;
}
