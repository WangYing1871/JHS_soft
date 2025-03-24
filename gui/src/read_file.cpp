#include <fstream>
#include <iostream>
#include "QTextBrowser"
#include "read_file.h"

void read_file::run(){
  std::ifstream fin(m_fname.c_str(),std::ios::binary);

  while(!fin.eof()){
    data_array_t buffer;
    buffer.resize(10*1024*1024);
    fin.read(buffer.data(),10*1024*1024);
    if (text_browser) text_browser->append("Readed");
    //std::cout<<"WY"<<std::endl;
  }
  fin.close();

}

read_file::read_file(std::string const& fname, QTextBrowser* browser):
  m_fname(fname)
  ,text_browser(browser){
}

read_file::read_file(){
}

read_file::~read_file(){
}
