#ifndef COMPARATOR_H_
#define COMPARATOR_H_

class Compare {
   public:
      bool operator() (std::string * lhs, std::string * rhs) {
    		return *lhs < *rhs;
      }
};

#endif /* COMPARATOR_H_ */