#include "Serialization/Serializatio.h"

#include <iostream>
#include <string>
#include <vector>

struct TestInner : public SerializationBase {
  SERIALIZATION_MEMBER(std::string, desc);
  SERIALIZATION_MEMBER(int, size);
};

struct Test : public SerializationBase {
  SERIALIZATION_MEMBER(std::string, desc);
  SERIALIZATION_MEMBER(std::vector<TestInner>, sub);
};

int main() {
  Test ori;
  ori.desc = "Test";
  TestInner sub;
  sub.desc = "TestInner";
  sub.size = 100;
  ori.sub.push_back(sub);
  sub.desc = "TestInner";
  sub.size = 102;
  ori.sub.push_back(sub);
  sub.desc = "TestInner";
  sub.size = 103;
  ori.sub.push_back(sub);
  Test copy = ori;
  nlohmann::json dat;
  copy.Serialize(dat);
  std::cout << dat.dump();

  Test out;
  out.Deserialize(dat);

  return 0;
}