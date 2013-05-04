#ifndef COMPARATOR_H_
#define COMPARATOR_H_

class Compare {
   public:
      bool operator() (std::string * lhs, std::string * rhs) {
    	  return lhs->compare(*rhs) < 0;
      }
};

#endif /* COMPARATOR_H_ */
