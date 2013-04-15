#ifndef DATABASE_H_
#define DATABASE_H_

#include <string>
#include <map>

class Compare {
   public:
      bool operator() (std::string * lhs, std::string * rhs) {
    		return *lhs < *rhs;
      }
};

class Database {
public:
	Database(const std::string& filename);
	~Database();
	int get_size() const;
	void get_sequence_by_name(const std::string& name, std::string & out_seq) const;
	std::map<std::string *, std::string *>::const_iterator get_data_iterator() const;
private:
	std::map<std::string *, std::string *, Compare> * data;
};


#endif /* DATABASE_H_ */
