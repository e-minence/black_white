#include "StdAfx.h"
#include "PokemonSearch.h"
#include "pokemontbl.h"

using namespace PokeIRC;

//--------------------------------------------------------------
/**
 * ポケモンの全国図鑑番号からさがす
 * @param   filename
 * @retval  StreamReader^
 */
//--------------------------------------------------------------

System::Void PokemonSearch::numericUpDown1_LocationChanged(System::Object^  sender, System::EventArgs^  e)
{
	int no = System::Decimal::ToInt32(numericUpDown1->Value);

	char* name = PokemonDataTable::getPokemonName(no);
	String^ pknm = gcnew String(name);
	textBox1->Text = pknm;
}


System::Void PokemonSearch::numericUpDown1_ValueChanged(System::Object^  sender, System::EventArgs^  e)
{
	int no = System::Decimal::ToInt32(numericUpDown1->Value);

	char* name = PokemonDataTable::getPokemonName(no);
	String^ pknm = gcnew String(name);
	textBox1->Text = pknm;
}
