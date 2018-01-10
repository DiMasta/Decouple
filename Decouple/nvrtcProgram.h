#ifndef __NVRTC_PROGRAM_H__
#define __NVRTC_PROGRAM_H__

#include <nvrtc.h>

#include <string>
#include <vector>

typedef std::vector<std::string> StringList;

class NVRTCProgram {
public:
	NVRTCProgram();
	~NVRTCProgram();

	std::string getMainProgramFileName() const;
	std::string getCudaProgramStr() const;
	int getHeadersCount() const;
	StringList getHeaderFilesSource() const;
	StringList getHeaderFilesNames() const;
	StringList getCompileOptions() const;
	nvrtcProgram getProgram() const;
	char* getPtx() const;
	size_t getPtxSize() const;

	void setMainProgramFileName(const std::string& mainProgramFileName);
	void setCudaProgarmStr(const std::string& cudaProgramStr);
	void setHeaderFilesSource(const StringList& headerFilesSource);
	void setHeaderFilesNames(const StringList& headerFilesNames);
	void setCompileOptions(const StringList& compileOptions);
	void setProgram(const nvrtcProgram& program);
	void setPtx(char* ptx);
	void setPtxSize(size_t ptxSize);

	void createProgram();
	void compileProgram();

	std::string readSourceCodeFile(const std::string& fileName) const;
	void readHeaderFiles(const std::string& fileName);
	void addCompileOption(const std::string& compileOption);
	void obtainCompileLog() const;
	void obtainPtx();
	void destroyProgram();

	void createAndCompile(const std::string& mainProgramFileName, const StringList& compileOptions);

private:
	std::string mainProgramFileName;
	std::string cudaProgramStr;
	int headersCount;
	StringList headerFilesSource;
	StringList headerFilesNames;
	StringList compileOptions;
	nvrtcProgram program;
	char* ptx;
	size_t ptxSize;
};

#endif //__NVRTC_PROGRAM_H__
