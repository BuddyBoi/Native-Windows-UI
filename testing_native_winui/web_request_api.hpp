#pragma once
#include "../../third_party_libs/httplib.h"
#include "../../third_party_libs/json.hpp"
#include <Windows.h>
#include <Psapi.h>
#include <vector>
#include <string>

namespace console
{
	class Console
	{
	public:
		std::string ReadLine()
		{
			if ( !_instance )
				return "";

			std::string input;
			std::getline( std::cin, input );
			return input;
		}
		void Write( const char* format, ... )
		{
			if ( !_instance )
				return;

			va_list args;
			va_start( args, format );
			vprintf( format, args );
			va_end( args );
		}
		bool Show()
		{
			if ( !_instance )
				return false;

			return ShowWindow( GetConsoleWindow(), SW_SHOW );
		}
		bool Hide()
		{
			if ( !_instance )
				return false;

			return ShowWindow( GetConsoleWindow(), SW_HIDE );
		}
		bool Deallocate()
		{
			if ( !_instance )
				return false;

			return FreeConsole();
		}
		bool Allocate()
		{
			if ( !_instance )
				return false;

			AllocConsole();

			freopen( "CONOUT$", "w", stdout );
			freopen( "CONIN$", "r", stdin );
			freopen( "CONERR$", "w", stderr );

			return true;
		}
		Console( int instance )
			: _instance( instance )
		{
			if ( !Allocate() )
				return;
		}
		Console()
		{
		}
	private:
		int _instance = 0;
	};
}

namespace weather_api
{
	std::string web_url_root = "http://historical-forecast-api.open-meteo.com";
	nlohmann::json json_data{};

	bool update( std::string input_date = "2024-11-26", std::string input_lat = "52.52", std::string input_long = "13.41" )
	{
		std::string web_input = "/v1/forecast?latitude=" + input_lat + "&longitude=" + input_long + "&start_date=" + input_date + "&end_date=" + input_date + "&hourly=temperature_2m&daily=temperature_2m_max,temperature_2m_min&temperature_unit=fahrenheit&wind_speed_unit=mph&precipitation_unit=inch&timeformat=unixtime&elevation_unit=feet";

		printf( "%s\n", web_input.c_str() );

		// Create a client connection
		httplib::Client cli( web_url_root );
		if ( !cli.is_valid() )
		{
			printf( "CLI BASE ROOT INVALID\n" );
			return false;
		}

		auto response = cli.Get( web_input );
		if ( !response )
		{
			printf( "INVALID RESPONSE\n" );
			return false;
		}
		if ( response->status != 200 )
		{
			printf( "INVALID STATUS\n" );
			return false;
		}

		//parse web input to JSON
		json_data = nlohmann::json::parse( response->body );
		if ( !json_data.size() )
		{
			printf( "INVALID JSON DATA\n" );
			return false;
		}

		return true;
	}

	float get_temperature_hour( int hour )
	{
		
	}

	float get_temperature_day_min()
	{
		const auto& daily_data = json_data["daily"];
		const auto& temp_max_array = daily_data["temperature_2m_max"];
		const auto& temp_min_array = daily_data["temperature_2m_min"];

		for ( size_t i = 0; i < temp_max_array.size(); ++i )
		{
			float max_temp = temp_max_array[i];
			float min_temp = temp_min_array[i];
			return min_temp;
		}
		return 0.0f;
	}

	float get_temperature_day_max()
	{
		const auto& daily_data = json_data["daily"];
		const auto& temp_max_array = daily_data["temperature_2m_max"];
		const auto& temp_min_array = daily_data["temperature_2m_min"];

		for ( size_t i = 0; i < temp_max_array.size(); ++i )
		{
			float max_temp = temp_max_array[i];
			float min_temp = temp_min_array[i];
			return max_temp;
			// Process the maximum and minimum temperatures for the ith day
			//std::cout << "Day " << i + 1 << ": Max Temp = " << max_temp << ", Min Temp = " << min_temp << std::endl;
		}

		return 0.0f;
	}

	std::string get_value( const std::string& key )
	{
		std::string ret = "";

		if ( !json_data.is_object()
			|| !json_data.contains( key ) )
		{
			printf( "FAILED OBJECT OR DOES NOT CONTAIN\n" );
			return "";
		}

		const auto& value = json_data[key];

		if ( value.is_null() )
		{
			printf( "VALUE IS NULL\n" );
			return "NULL";
		}
		if ( value.is_string() )
		{
			return value.get<std::string>();
		}
		if ( value.is_number() )
		{
			int num = value.get<int>();
			ret = std::to_string( num );
			return ret;
		}
		if ( value.is_number_float() )
		{
			float num = value.get<float>();
			ret = std::to_string( num );
			return ret;
		}

		return "ERROR";
	}

	void test_request()
	{
		if ( !update("2024-12-11", "35.408669", "-80.579178") )
		{
			printf( "UPDATE FAILED\n" );
			Sleep( 1000 );
			return;
		}

		std::string s_temp_noon = get_value( "elevation" );
		printf( "ELEVATION: %s\n", s_temp_noon.c_str() );

		float temp_min = get_temperature_day_min();
		float temp_max = get_temperature_day_max();
		printf( "TEMP MIN: %0.01f, TEMP MAX: %0.01f\n", temp_min, temp_max );
	}

	void test_123()
	{
		console::Console console( 1 );
		console.Allocate();
		console.Show();
		test_request();
	}
}