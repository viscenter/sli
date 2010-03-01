#pragma once
#include <Windows.h>

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;


namespace projectorController {

	/// <summary>
	/// Summary for gdiForm
	///
	/// WARNING: If you change the name of this class, you will need to change the
	///          'Resource File Name' property for the managed resource compiler tool
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	public ref class gdiForm : public System::Windows::Forms::Form
	{
	public:
		gdiForm(void)
		{
			InitializeComponent();
			//
			//TODO: Add the constructor code here
			//
		}

		gdiForm(DEVMODE* monitor)
		{
			InitializeComponent();
			this->initToScreen(monitor);
		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~gdiForm()
		{
			if (components)
			{
				delete components;
			}
		}

	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>
		System::ComponentModel::Container ^components;

#pragma region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			this->SuspendLayout();
			// 
			// gdiForm
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->BackColor = System::Drawing::Color::White;
			this->ClientSize = System::Drawing::Size(800, 600);
			this->ControlBox = false;
			this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::None;
			this->Name = L"gdiForm";
			this->Text = L"gdiForm";
			this->TopMost = true;
			this->ResumeLayout(false);

		}
#pragma endregion

	public: void initToScreen(DEVMODE* monitor)
			{
				this->Visible = true;
				this->Left = monitor->dmPosition.x;
				this->Top = monitor->dmPosition.y;
				this->Height = monitor->dmPelsHeight;
				this->Width = monitor->dmPelsWidth;
				this->Visible = false;
			}
	public: void hidePattern()
			{
				this->Visible = false;
			}
	public: void showPattern(int patternNum)
			{
				this->Visible = true;
				Graphics^ graphics = this->CreateGraphics();
				
				Bitmap ^pattern1 = gcnew Bitmap("pattern" + patternNum + ".bmp");
				
				TextureBrush ^brush = gcnew TextureBrush(pattern1);
				graphics->FillRectangle(brush, 0, 0, this->Width, this->Height);
				
				//Test code to actually turn off the projector....
				//HWND hwnd = GetForegroundWindow();
				//::SendMessage(hwnd, (UINT)WM_SYSCOMMAND, (WPARAM)SC_MONITORPOWER, (LPARAM)2);
			}
	};
}
