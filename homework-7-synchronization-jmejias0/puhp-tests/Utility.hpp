#ifndef PUHP_TESTS_UTILITY_HPP
#define PUHP_TESTS_UTILITY_HPP


//
#include <iostream>
#include <limits>
#include <sstream>
#include <vector>


//
namespace PuhPTests
{
	//
	class Utility
	{
		//
		public:
			
			//
			Utility()
			{
				//
			}
			
			static void clearCinBuffer()
			{
				if ( std::cin.rdbuf()->in_avail() > 0 ) {
					std::cin.clear();	// clear must come before ignore lol
					std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
				}
			}
			
			//
			static std::string toString(const std::vector<std::string>& v)
			{
				//
				std::stringstream ss;
				
				//
				ss << "{";
				for ( size_t i = 0 ; i < v.size(); i++ ) {
					
					//
					if ( i > 0 ) {
						ss << ", ";
					}
					ss << v[i];
				}
				ss << "}";
				
				return ss.str();
			}
			
			//
			static std::string toString(const std::vector<int>& v)
			{
				//
				std::stringstream ss;
				
				//
				ss << "{";
				for ( size_t i = 0 ; i < v.size(); i++ ) {
					
					//
					if ( i > 0 ) {
						ss << ", ";
					}
					ss << v[i];
				}
				ss << "}";
				
				return ss.str();
			}
		
		//
		private:
			
			//
			
	};
}












#endif
