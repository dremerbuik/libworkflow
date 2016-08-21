#ifndef __JSONABLE_H_
#define __JSONABLE_H_
#include <boost/property_tree/ptree.hpp>

/**
 This class allow other to be serialized to JSON format.
 Seek boost property tree doc for more infos.
 
 Simply override save() and load().
 */
class Jsonable  {
public:
    Jsonable();
    virtual ~Jsonable();
    
    //! store content of this class to a json string.
    void save(std::string &) const;
    //! load the content of this class from a json string
    void load(const std::string &);
    
    //! this does the actual class to ptree conversion
    virtual void save(boost::property_tree::ptree & root) const =0;
    //! this convert a string to a ptree
    virtual void load(const boost::property_tree::ptree & root) =0;
    
};


#endif // __JSONABLE_H_