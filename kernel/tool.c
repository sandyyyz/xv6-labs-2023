/*
define some tool function we may need for our module
*/
inline unsigned long ROUNDDOWN2(unsigned long number) {
  if (number <= 0)
    return -1;
  if (is_power2(number))
    return number;
  int ret = 1;
  while (ret <= number)
    ret <<= 1;
  return ret >> 1;
}
inline unsigned long ROUNDUP2(unsigned long number) {
  if (number < 0)
    return -1;
  if (is_power2(number))
    return number;
  int ret = 1;
  while (ret <= number)
    ret << 1;
  return ret >> 1;
}

inline int is_power2(unsigned long number) {
  return ((number != 0) && (number & (number - 1) == 0)) ? 1 : 0;
}
inline unsigned long get_order_2(unsigned long n) {
  unsigned long order = 0;
  while (n >> 1) {
    n >>= 1;
    order++;
  }
  return order;
}
