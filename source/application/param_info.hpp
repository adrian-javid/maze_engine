#ifndef Application_ParamInfo_hpp
#define Application_ParamInfo_hpp

#include <string>
#include <unordered_map>
#include <optional>
#include "linear_interpolation.hpp"
#include <sstream>
#include "print.hpp"

namespace App {
	struct ParamInfo {
		std::string const description;
		std::string argument;
		using Acceptable = std::unordered_map<std::string, std::string>;
		std::optional<Acceptable> const acceptable;
		static std::unordered_map<std::string, ParamInfo> const &parseArgv(int const argc, char const *const *const argv);

		static constexpr char const *helpTipString{"Run with `help` for information about this program."};

		template <typename T>
		static T assertNonnegative(T const value, std::string const &errorMessage=std::string()) {
			static_assert(std::is_signed_v<T>);
			if (value < T{0}) errorExit((std::ostringstream() << errorMessage <<
				"\n\tValue `" << value << "` should not be negative.").str()
			);
			return value;
		}

		template <typename T>
		static T assertPositive(T const value, std::string const &errorMessage=std::string()) {
			static_assert(std::is_signed_v<T>);
			if (not (value > T{0})) errorExit((std::ostringstream() << errorMessage <<
				"\n\tValue `" << value << "` should be positive.").str()
			);
			return value;
		}

		template<typename T>
		static T castArg(std::string const &arg) {
			static_assert(
				std::is_same_v<T, int> or
				std::is_same_v<T, unsigned int> or
				std::is_same_v<T, bool>,
				"Unsupported type."
			);

			static constexpr auto getTypeName([]() -> std::string {
				if constexpr (std::is_same_v<T, int>)
					return "int";
				else if constexpr (std::is_same_v<T, unsigned int>)
					return "unsigned int";
				else if constexpr (std::is_same_v<T, bool>)
					return "bool";
				else
					return "";
			});

			T value{};
			std::istringstream stream(arg);
			if ((stream >> std::boolalpha >> value).fail()) errorExit(
				"Unable to convert `", arg, "` to `", getTypeName(), "` value. \n\n", helpTipString
			);

			return value;
		}

		static std::string validParametersStr(bool const verbose=false);

		static std::string acceptableValuesStr(ParamInfo::Acceptable const &acceptableValues);

		private:
			static std::unordered_map<std::string, ParamInfo> config;
	};
}

#endif
