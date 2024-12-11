#include <windows.h>
#include <string>
#include <vector>

namespace vars
{
	HWND hwnd = nullptr;
}

namespace util
{
	//testing namespace for simplification and translation of SendMessage in different contexts and controls
	namespace send_message
	{
		namespace listbox
		{
			bool add( HWND object, std::string msg )
			{
				return (SendMessageA( object, LB_ADDSTRING, 0, (LPARAM)msg.c_str() ) == 0);
			}

			bool remove( HWND object, int index )
			{
				return (SendMessageA( object, LB_DELETESTRING, index, 0 ) == 0);
			}

			bool remove( HWND object, std::string msg )
			{
				int index = SendMessageA( object, LB_FINDSTRINGEXACT, 0, (LPARAM)msg.c_str() );
				if ( index != LB_ERR )
				{
					int result = SendMessageA( object, LB_DELETESTRING, index, 0 );
					return (result == 0);
				}

				return false;
			}

			int clear( HWND object )
			{
				return (SendMessageA( object, LB_RESETCONTENT, 0, 0 ) == 0);

			}

			int get_selected_index( HWND object )
			{
				return SendMessageA( object, LB_GETCURSEL, 0, 0 );
			}

			std::string get_selected_text( HWND object )
			{
				int index = get_selected_index( object );
				if ( index == LB_ERR )
					return "";

				// Get the selected item's text
				char buffer[256];
				SendMessageA( object, LB_GETTEXT, index, (LPARAM)buffer );
				return std::string( buffer );
			}

			std::string get_text( HWND object, int index )
			{
				char buffer[256];
				SendMessageA( object, LB_GETTEXT, index, (LPARAM)buffer );
				return std::string( buffer );
			}

			int get_count( HWND object )
			{
				return SendMessageA( object, LB_GETCOUNT, 0, 0 );
			}

			bool sort( HWND object )
			{
				return true;
				//SendMessageA( object, LB_SORT, 0, 0 );
			}

			std::vector<std::string> get_items( HWND object )
			{
				std::vector<std::string> ret{};

				int count = get_count( object );
				for ( int i = 0; i != count; i++ )
				{
					std::string current_item = get_text( object, i );
					ret.push_back( current_item );
				}

				return ret;
			}
		}

		namespace textbox
		{
			std::string get( HWND object )
			{
				char buffer[256];
				GetWindowTextA( object, buffer, 256 );
				return std::string( buffer );
			}

			bool set( HWND object, std::string msg )
			{
				return SetWindowTextA( object, msg.c_str() );
			}

			bool clear( HWND object )
			{
				return SetWindowTextA( object, "" );
			}
		}

		namespace button
		{
			bool enable( HWND object )
			{
				return EnableWindow( object, TRUE );
			}

			bool disable( HWND object )
			{

				return EnableWindow( object, FALSE );
			}
		}
	}

	namespace window
	{
		enum E_CONTROL_TYPE
		{
			CONTROL_TYPE_INVALID,
			CONTROL_TYPE_BUTTON,
			CONTROL_TYPE_LISTBOX,
			CONTROL_TYPE_TEXTBOX,
			CONTROL_TYPE_LABEL,
			CONTROL_TYPE_MAX,
		};

		struct T_CONTROL
		{
			int id = 0;
			std::string name = "";
			HWND ptr = nullptr;
			E_CONTROL_TYPE type = CONTROL_TYPE_INVALID;
		};

		std::vector<T_CONTROL> vec_controls{};

		void add_control( E_CONTROL_TYPE type, int size_x, int size_y, std::string name, HWND wnd, std::string label = "NONE" )
		{
			int index = vec_controls.size();

			static int i_last_pos_x = 0;
			static int i_last_pos_y = 0;
			static int i_last_size_y = 0;

			if ( type == CONTROL_TYPE_BUTTON ) //button
			{
				int y = i_last_pos_y + i_last_size_y + 10;

				HWND object = CreateWindowExA( WS_EX_CLIENTEDGE, "BUTTON", label.c_str(),
					WS_CHILD | WS_VISIBLE | WS_BORDER,
					i_last_pos_x, y, size_x, size_y,
					wnd, (HMENU)index, 0, NULL );

				i_last_pos_y = y;
				i_last_size_y = size_y;

				T_CONTROL control{};
				control.id = index;
				control.name = name;
				control.ptr = object;
				control.type = type;
				vec_controls.push_back( control );
			}
			else if ( type == CONTROL_TYPE_LISTBOX ) //listbox
			{
				int y = i_last_pos_y + i_last_size_y + 10;

				HWND object = CreateWindowExA( WS_EX_CLIENTEDGE, "LISTBOX", NULL,
					WS_CHILD | WS_VISIBLE | WS_VSCROLL | LBS_NOTIFY,
					i_last_pos_x, y, size_x, size_y,
					wnd, (HMENU)index, 0, NULL );

				i_last_pos_y = y;
				i_last_size_y = size_y;

				T_CONTROL control{};
				control.id = index;
				control.name = name;
				control.ptr = object;
				control.type = type;
				vec_controls.push_back( control );
			}
			else if ( type == CONTROL_TYPE_TEXTBOX ) //textbox
			{
				int y = i_last_pos_y + i_last_size_y + 10;

				HWND object = CreateWindowExA( WS_EX_CLIENTEDGE, "EDIT", NULL,
					WS_CHILD | WS_VISIBLE | WS_BORDER,
					i_last_pos_x, y, size_x, size_y,
					wnd, (HMENU)index, 0, NULL );

				i_last_pos_y = y;
				i_last_size_y = size_y;

				T_CONTROL control{};
				control.id = index;
				control.name = name;
				control.ptr = object;
				control.type = type;
				vec_controls.push_back( control );
			}
			else if ( type == CONTROL_TYPE_LABEL ) //label
			{
				//label
			}
			else
			{
				//else
			}
		}

		T_CONTROL get_control( int index )
		{
			return vec_controls[index];
		}

		T_CONTROL get_control( std::string name )
		{
			for ( T_CONTROL current : vec_controls )
			{
				if ( current.name == name )
					return current;
			}
			return T_CONTROL{};
		}

		LRESULT CALLBACK WndProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
		{
			switch ( message )
			{
				case WM_CREATE:
				{
					//create items

					//null WINDOW item
					T_CONTROL null_control{};
					null_control.id = 0;
					null_control.name = "MAIN";
					null_control.ptr = vars::hwnd;
					vec_controls.push_back( null_control );

					add_control( CONTROL_TYPE_LISTBOX, 200, 200, "LISTBOX_DATA", hWnd );
					add_control( CONTROL_TYPE_TEXTBOX, 200, 20, "TEXT_INPUT", hWnd );
					add_control( CONTROL_TYPE_BUTTON, 200, 20, "BUTTON_ADD", hWnd, "Add" );
					add_control( CONTROL_TYPE_BUTTON, 200, 20, "BUTTON_SUB", hWnd, "Remove" );
					add_control( CONTROL_TYPE_BUTTON, 200, 20, "BUTTON_CLR", hWnd, "Clear" );
					add_control( CONTROL_TYPE_TEXTBOX, 200, 20, "TEXT_INPUT_USERNAME", hWnd );
					add_control( CONTROL_TYPE_TEXTBOX, 200, 20, "TEXT_INPUT_PASSWORD", hWnd );
					add_control( CONTROL_TYPE_BUTTON, 200, 20, "BUTTON_LOGIN", hWnd, "Login" );

					T_CONTROL ctr_textbox = get_control( "TEXT_INPUT" );

					// Set focus to the textbox
					SetFocus( ctr_textbox.ptr );
					break;
				}
				case WM_COMMAND:
				{
					int index = LOWORD( wParam ); //component interaction by MENU ID (defined in object creation)
					T_CONTROL control = get_control( index );
					if ( control.name == "BUTTON_ADD" )
					{
						T_CONTROL ctr_textbox = get_control( "TEXT_INPUT" );
						T_CONTROL ctr_listbox = get_control( "LISTBOX_DATA" );
						if ( !ctr_textbox.ptr
							|| !ctr_listbox.ptr )
							break;

						//get textbox text
						std::string textbox_input = util::send_message::textbox::get( ctr_textbox.ptr );
						if ( !textbox_input.size() )
							break;

						//add text to ListBox
						util::send_message::listbox::add( ctr_listbox.ptr, textbox_input );

						//clear textbox
						util::send_message::textbox::clear( ctr_textbox.ptr );
						break;
					}
					else if ( control.name == "BUTTON_SUB" )
					{
						T_CONTROL ctr_textbox = get_control( "TEXT_INPUT" );
						T_CONTROL ctr_listbox = get_control( "LISTBOX_DATA" );
						if ( !ctr_textbox.ptr
							|| !ctr_listbox.ptr )
							break;

						//get textbox text
						std::string textbox_input = util::send_message::textbox::get( ctr_textbox.ptr );

						if ( textbox_input.size() == 0 )
						{
							//if textbox empty, just clear selected item
							util::send_message::listbox::remove( ctr_listbox.ptr, util::send_message::listbox::get_selected_index( ctr_listbox.ptr ) );
						}
						else
						{
							//remove text from ListBox
							util::send_message::listbox::remove( ctr_listbox.ptr, textbox_input );

							//clear textbox
							util::send_message::textbox::clear( ctr_textbox.ptr );
						}
					}
					else if ( control.name == "BUTTON_CLR" )
					{
						T_CONTROL ctr_textbox = get_control( "TEXT_INPUT" );
						T_CONTROL ctr_listbox = get_control( "LISTBOX_DATA" );
						if ( !ctr_textbox.ptr
							|| !ctr_listbox.ptr )
							break;

						//clear listbox
						util::send_message::listbox::clear( ctr_listbox.ptr );

						//clear textbox
						util::send_message::textbox::clear( ctr_textbox.ptr );
					}
					else if ( control.name == "BUTTON_LOGIN" )
					{
						T_CONTROL ctr_textbox_username = get_control( "TEXT_INPUT_USERNAME" );
						T_CONTROL ctr_textbox_password = get_control( "TEXT_INPUT_PASSWORD" );
						T_CONTROL ctr_listbox = get_control( "LISTBOX_DATA" );
						if ( !ctr_textbox_username.ptr
							|| !ctr_textbox_password.ptr
							|| !ctr_listbox.ptr)
							break;

						//get username and password
						std::string s_username = "";
						std::string s_password = "";

						s_username = util::send_message::textbox::get( ctr_textbox_username.ptr );
						if ( !s_username.size() )
							break;

						s_password = util::send_message::textbox::get( ctr_textbox_password.ptr );
						if ( !s_password.size() )
							break;

						//check username and password
						if ( s_username != "buddy" )
						{
							util::send_message::listbox::add( ctr_listbox.ptr, "ERR: INVALID USER" );
							break;
						}

						if ( s_password != "Buddy30" )
						{
							util::send_message::listbox::add( ctr_listbox.ptr, "ERR: INVALID PASS" );
							break;
						}

						util::send_message::listbox::add( ctr_listbox.ptr, "SUCCESSFUL LOGIN" );
					}
				}
				break;

				case WM_DESTROY:
					PostQuitMessage( 0 );
					break;

				default:
					return DefWindowProcA( hWnd, message, wParam, lParam );
			}
			return 0;
		}

		HWND create( std::string w_title, std::string w_class, int w_size_x, int w_size_y, HINSTANCE instance )
		{
			WNDCLASSEXA wc = { 0 };
			wc.cbSize = sizeof( wc );
			wc.lpfnWndProc = WndProc;
			wc.hInstance = instance;
			wc.lpszClassName = w_class.c_str();

			RegisterClassExA( &wc );

			vars::hwnd = CreateWindowExA( WS_EX_CLIENTEDGE, w_class.c_str(), w_title.c_str(),
				WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, w_size_x, w_size_y,
				NULL, NULL, instance, NULL );

			return vars::hwnd;
		}

		void loop( int cmdshow )
		{
			ShowWindow( vars::hwnd, cmdshow );
			UpdateWindow( vars::hwnd );

			MSG msg;
			while ( GetMessageA( &msg, NULL, 0, 0 ) )
			{
				if ( msg.message == WM_QUIT )
				{
					break;
				}
				TranslateMessage( &msg );
				DispatchMessageA( &msg );				
			}
		}
	}
}

int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow )
{
	vars::hwnd = util::window::create( "Buddy UI", "BuddyUIWindow", 800, 600, hInstance );

	util::window::loop( nCmdShow );
	return 0;
}