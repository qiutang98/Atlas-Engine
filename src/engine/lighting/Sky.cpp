#include "Sky.h"

namespace Atlas {

	namespace Lighting {

		Sky::Sky() {



		}

		EnvironmentProbe* Sky::GetProbe() {

			// Prioritize user loaded cubemaps
			if (probe) return probe;
			if (atmosphere) return &atmosphere->probe;

			return nullptr;

		}

	}

}