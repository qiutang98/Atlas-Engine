#ifndef AE_EXTENSIONS_H
#define AE_EXTENSIONS_H

#include "System.h"

#include <unordered_set>

namespace Atlas {

	class Extensions {

	public:
		/**
		 * Processes all API extensions
		 * @note This is automatically called at engine startup
		 */
		static void Process();

		/**
		 * Checks whether an extension is supported
		 * @param extension The extension to be checked
		 * @return True if the extension is supported, false otherwise
		 */
		static bool IsSupported(std::string extension);

	private:
		static std::unordered_set<std::string> supportedExtensions;

	};

}

#endif