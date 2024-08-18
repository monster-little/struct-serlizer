#include "Serialization/Serializatio.h"

#include <iostream>
#include <string>
#include <vector>

struct TestInner : public SerializationBase {
  SERIALIZATION_MEMBER(std::string, desc);
  SERIALIZATION_MEMBER(int, size);
};

struct TestInner2 : public SerializationBase {
  SERIALIZATION_MEMBER(std::string, desc);
  SERIALIZATION_MEMBER(float, size);
};

struct Test : public SerializationBase {
  SERIALIZATION_MEMBER(std::string, desc);
  SERIALIZATION_MEMBER(std::vector<TestInner>, sub);
};

struct Test2 : public SerializationBase {
  SERIALIZATION_MEMBER(std::string, desc);
  SERIALIZATION_MEMBER(std::string, sub);
};

struct Test3 : public SerializationBase {
  SERIALIZATION_MEMBER(std::string, desc);
  SERIALIZATION_MEMBER(std::vector<TestInner2>, sub);
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

  //obj dump to string
  Test2 out2;
  out2.Deserialize(dat);

  TestInner ti3;
  ti3.desc = "TestInner";
  ti3.size = 100;
  nlohmann::json ti3_out;
  ti3.Serialize(ti3_out);

  TestInner2 out3;
  out3.Deserialize(ti3_out);

  return 0;
}