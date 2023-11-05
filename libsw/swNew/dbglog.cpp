
#include "dbglog.h"


void	swLogOutputStream::write(const char * text){
	write( (swByte*)text,(swUInt32)strlen(text));
}

void	swLogOutputStream::writeln(const char* text){
	write(text);
	write("\n");
}


void	swLogStdoutStream::write(const char* text){
	printf(text);
}

swLogFileStream::swLogFileStream(const swString& file,char * mode){
	if( mode == std::string("a")){
		_file.Create(file,false);
		_file.SeekEnd();
	}else{
		_file.Create(file,true);
	}
}

void	swLogFileStream::write(const swByte* bytes,swUInt32 size){
	if(_file.IsOpened() == false){
		printf("logfile cann't write!\n");
		return;
	}
	_file.Write((void*)bytes,(size_t)size);
}