#ifndef PUHP_TESTS_INSTRUCTION_COUNTER_HPP
#define PUHP_TESTS_INSTRUCTION_COUNTER_HPP


//
#include "./Files.hpp"
#include "./Process.hpp"


//
#include <regex>
#include <string>


//
namespace PuhPTests
{
	//
	class InstructionCounter
	{
		//
		public:
			
			static constexpr const char * CALLGRIND_TMPFILE_NAME = "callgrind.tmp";
			
			//
			InstructionCounter()
			{
				//
			}
			
			//
			static bool countExecutionInstructions(
				std::vector<std::string> compile_command, std::string exe_name,
				std::string& out_stdout, std::string& out_stderr, std::string& out_message,
				unsigned long long& out_instruction_count
			)
			{
				//	Default the count to the worst possible value
				out_instruction_count = 0;
				out_instruction_count--;
				
				//	Check that valgrind is actually installed
				{
					Process p;
					
					if ( p.programExists("valgrind") == false ) {
						throw std::runtime_error("Cannot count execution instructions because valgrind is not installed !");
					}
				}
				
				//	Attempt to compile
				{
					Process p;
					ExecutionResult result = p.execute(compile_command);
					
					//	Only continue if compilation succeeds
					if ( result.code != 0 ) {
						
						out_stdout = result.stdout;
						out_stderr = result.stderr;
						out_message = "Compilation failed";
						
						return false;
					}
				}
				
				//	Execute valgrind::callgrind, to count instructions
				{
					Process p;
					ExecutionResult result = p.execute(
						{
							"valgrind",
							"--tool=callgrind",
							"--callgrind-out-file=./" + std::string(InstructionCounter::CALLGRIND_TMPFILE_NAME),
							"--error-exitcode=111",
							"./" + exe_name
						}
					);
					
					out_stdout = result.stdout;
					out_stderr = result.stderr;
					
					if ( result.code != 0 ) {
						
						out_message = "Unable to count instructions.";
						
						return false;
					}
				}
				
				//	Execute valgrind to annotate the callgring output
				//	This step cannot be skipped, as the original output is a binary file with strange encoding,
				//		even though it looks like a text file.
				std::string callgrind_annotated;
				{
					Process p;
					ExecutionResult result = p.execute(
						{
							"callgrind_annotate",
							"--auto=no",			// auto=yes produces unneeded information and is way too slow
							"./" + std::string(InstructionCounter::CALLGRIND_TMPFILE_NAME),
						}
					);
					
					out_stdout = result.stdout;
					out_stderr = result.stderr;
					
					callgrind_annotated = out_stdout;
					
					if ( result.code != 0 ) {
						out_message = "Unable to annotate callgrind output file.";
						return false;
					}
				}
				
				//	Remove the tmp file
				PuhPTests::Files::removeFile(
					"./" + std::string(InstructionCounter::CALLGRIND_TMPFILE_NAME),
					false
				);
				
				//	Callgrind seems to output \r\n line endings,
				//	which don't work with regex's $ token.
				callgrind_annotated.erase(
					std::remove(
						callgrind_annotated.begin(), callgrind_annotated.end(), '\r'
					),
					callgrind_annotated.end()
				);
				out_stdout = callgrind_annotated;
				
				//	Use regex to locate the instruction count
				std::regex totals_regex(
					(
						"^-{10,}$\n"
						"^Ir\\s*$\n"
						"^-{10,}$\n"
						"([0-9,]+) \\([0-9\\.]+%\\)\\s+PROGRAM\\s+TOTALS"
					),
					std::regex_constants::multiline
				);
				std::smatch totals_match;
				bool found_match = std::regex_search(callgrind_annotated, totals_match, totals_regex);
				
				//
				if ( !found_match ) {
					out_message = "Unable to parse callgrind-annotated results for total instruction count.";
					return false;
				}
				
				//	Grab the instruction count total, without commas
				std::string instruction_count_s = totals_match[1];
				instruction_count_s.erase(
					std::remove(
						instruction_count_s.begin(), instruction_count_s.end(), ','
					),
					instruction_count_s.end()
				);
				out_instruction_count = std::stoll(instruction_count_s);
				
				//
				out_message = "Successfully counted instructions";
				
				return true;
			}
		
		//
		private:
			
			//
	};
}


#endif


