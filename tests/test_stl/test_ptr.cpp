#include <memory>
#include <iostream>

int main() {
  std::cout << sizeof(std::unique_ptr<int>) << "\n";
  std::cout << sizeof(std::shared_ptr<int>) << "\n";
  return 0;
}
