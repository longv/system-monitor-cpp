#ifndef PROCESSOR_H
#define PROCESSOR_H

class Processor {
 public:
  float Utilization(); 

 private:
  long prevActiveJiffies_;
  long prevTotalJiffies_;
};

#endif