/***************************************************************************
                   prjoptionsdlg.cpp - the setup DLG for a project  
                             -------------------                                         

    begin                : 10 Aug 1998                                        
    copyright            : (C) 1998 by Sandy Meier,Stefan Bartel
    email                : smeier@rz.uni-potsdam.de,bartel@rz.uni-potsdam.de                                     
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   * 
 *                                                                         *
 ***************************************************************************/

#include "cprjoptionsdlg.h"
#include "vc/versioncontrol.h"
#include <iostream.h>
#include <qstrlist.h>
#include <klocale.h>
#include "debug.h"

// OPTIONS DIALOG
CPrjOptionsDlg::CPrjOptionsDlg(CProject* prj, QWidget *parent, const char *name)
    : QTabDialog( parent, name,TRUE )
{
  prj_info = prj;
  QString cxxflags=prj->getCXXFLAGS();
  QString ldflags=prj->getLDFLAGS();
  QString ldadd=prj->getLDADD();
  
  old_version = prj->getVersion();
  old_ldflags =  ldflags.stripWhiteSpace();
  old_ldadd = ldadd.stripWhiteSpace();
  old_addit_flags = prj->getAdditCXXFLAGS().stripWhiteSpace();
  old_cxxflags = cxxflags.stripWhiteSpace();

  need_configure_in_update = false;
  need_makefile_generation = false;

  
  setFixedSize(560,355);
  QStrList short_info;
  int pos;

  setCaption( i18n("Project Options" ));
  
  // ****************** the General_Widget ********************
  QWidget *w = new QWidget( this, "general" );
  KQuickHelp::add(w, i18n("Set the general options of your project here."));

  QLabel* prjname_label;
  prjname_label = new QLabel( w, "prjname_label" );
  prjname_label->setGeometry( 10, 10, 170, 20 );
  prjname_label->setText( i18n("Project name:") );
  
  prjname_edit = new QLineEdit( w, "prjname_edit" );
  prjname_edit->setGeometry( 10, 30, 170, 30 );
  prjname_edit->setText( prj_info->getProjectName() );
  KQuickHelp::add(prjname_label,
  KQuickHelp::add(prjname_edit, i18n("Set the project name here.")));
  

  QLabel* version_label;
  version_label = new QLabel( w, "version_label" );
  version_label->setGeometry( 190, 10, 60, 20 );
  version_label->setText( i18n("Version:") );

  version_edit = new QLineEdit( w, "version_edit" );
  version_edit->setGeometry( 190, 30, 60, 30 );
  version_edit->setText( prj_info->getVersion() );
  KQuickHelp::add(version_label,
  KQuickHelp::add(version_edit, i18n("Set your project version\n"
				     "number here.")));


  QLabel* author_label;
  author_label = new QLabel( w, "author_label" );
  author_label->setGeometry( 10, 80, 240, 20 );
  author_label->setText( i18n("Author:") );
   
  author_edit = new QLineEdit( w, "author_edit" );
  author_edit->setGeometry( 10, 100, 240, 30 );
  author_edit->setText( prj_info->getAuthor() );
  KQuickHelp::add(author_label,
  KQuickHelp::add(author_edit, i18n("Insert your name or the name of your team here")));
   


  QLabel* email_label;
  email_label = new QLabel( w, "email_label" );
  email_label->setGeometry( 10, 150, 240, 20 );
  email_label->setText( i18n("Email:") );
  
  email_edit = new QLineEdit( w, "email_edit");
  email_edit->setGeometry( 10, 170, 240, 30 );
  email_edit->setText( prj_info->getEmail() );
  KQuickHelp::add(email_label,
  KQuickHelp::add(email_edit, i18n("Insert your email-address here")));


  modifymakefiles_checkbox = new QCheckBox( w, "" );
  modifymakefiles_checkbox->setGeometry( 10, 215, 290, 20 );
  modifymakefiles_checkbox->setText( i18n("Modify Makefiles") );
  modifymakefiles_checkbox->setChecked(prj->getModifyMakefiles());

  QLabel *vcsystem_label 
    = new QLabel( i18n("Version Control:"), w, "vcsystem_label" );
  vcsystem_label->setGeometry( 10, 240, 170, 30 );

  vcsystem_combo = new QComboBox( false, w );
  vcsystem_combo->setGeometry( 190, 240, 60, 30 );
  QStrList l;
  VersionControl::getSupportedSystems(&l);
  vcsystem_combo->insertItem(i18n("None"));
  vcsystem_combo->insertStrList(&l);
  QString vcsystem = prj_info->getVCSystem();
  for (int i = 0; i < vcsystem_combo->count(); ++i)
      if (vcsystem_combo->text(i) == vcsystem)
          vcsystem_combo->setCurrentItem(i);

  QLabel* info_label;
  info_label=new QLabel(w,"info_label");
  info_label->setGeometry(270,10,225,20);
  info_label->setText(i18n("Short Information:"));

  info_edit=new QMultiLineEdit(w,"info_edit");
  info_edit->setGeometry(270,30,270,240);
  short_info=prj_info->getShortInfo();
  short_info.first();
  do {
    info_edit->append(short_info.current());
  } while(short_info.next());
  KQuickHelp::add(info_label,
  KQuickHelp::add(info_edit, i18n("Insert some useful information about\n"
				  "your project here. This is only for the\n"
				  "information in the project file-<b>not\n"
				  "\nfor README etc.")));  


  addTab( w, i18n("General"));

  // *************** Compiler options *********************

  QWidget *w2= new QWidget(this,"Compiler options");
  KQuickHelp::add(w2, i18n("Set your Compiler options here"));

  QGroupBox* target_group;
  target_group=new QGroupBox(w2,"target_group");
  target_group->setGeometry(10,10,260,140);
  target_group->setTitle(i18n("Target"));
  KQuickHelp::add(target_group, i18n("Set your target options here\n"
				     "by specifying your machine type\n"
				     "and GCC optimization level (0-3)"));

  QLabel* target_label;
  target_label=new QLabel(w2,"target_label");
  target_label->setGeometry(20,30,100,20);
  target_label->setText(i18n("Target Machine"));

  target=new QComboBox(false,w2,"target");
  target->setGeometry(120,30,120,20);
  target->insertItem(i18n("your machine"),0);
  target->insertItem(i18n("i386v"),1);
  if (cxxflags.contains("-b ")) {
    if (cxxflags.contains("i386v")) {
      target->setCurrentItem(1);
    }
  } else {
    target->setCurrentItem(0);
  }
  KQuickHelp::add(target_label,
  KQuickHelp::add(target, i18n("Specify the machine type for your program.\n"
			       "Other machine types than your machine is\n"
			       "usually only needed for precompiled\n"
			       "distribution packages. i386v is intended for\n"
			       "cross-compilers to build a binary for an intel\n"
			       "machine running Unix System V.")));

  // syntax_check=new QCheckBox(w2,"syntax_check");
//   syntax_check->setGeometry(20,60,220,20);
//   syntax_check->setText(i18n("only syntax check"));
//   syntax_check->setChecked(cxxflags.contains("-fsyntax-only"));
//   KQuickHelp::add(syntax_check, i18n("This option sets the compiler\n"
// 			     	"to <i>-fsyntax-only</i>\n"
// 				"which lets you check your code for\n"
// 				"syntax-errors but doesn't do anything\n"
// 				"else beyond that.")); 

  optimize=new QCheckBox(w2,"optimize");
  optimize->setGeometry(20,90,220,20);
  optimize->setText(i18n("optimize"));
  optimize->setChecked(!cxxflags.contains("-O0"));

  optimize_level=new QSpinBox(w2,"optimize_level");
  optimize_level->setGeometry(40,120,40,20);
  optimize_level->setRange(1,3);
  if (cxxflags.contains("-O1")) optimize_level->setValue(1);
  if (cxxflags.contains("-O2")) optimize_level->setValue(2);
  if (cxxflags.contains("-O3")) optimize_level->setValue(3);
  connect( optimize_level, SIGNAL(valueChanged(int)),this , SLOT(slotOptimize_level_changed(int)) );
  
  QLabel* optimize_level_label;
  optimize_level_label=new QLabel(w2,"optimize_level_label");
  optimize_level_label->setGeometry(100,120,140,20);
  optimize_level_label->setText(i18n("Optimization-level"));
  KQuickHelp::add(optimize,
  KQuickHelp::add(optimize_level,
  KQuickHelp::add(optimize_level_label, i18n("Set the -O option for the GCC\n"
  					"here. Turning off optimization\n"
				     	"equals -O0. The higher the level\n"
					"the more time you need to compile\n"
				 	"but increases program speed."))));

  QGroupBox* debug_group;
  debug_group=new QGroupBox(w2,"debug_group");
  debug_group->setGeometry(280,10,255,140);
  debug_group->setTitle(i18n("Debugging"));
  KQuickHelp::add(debug_group, i18n("Set your debugging options here."));

  debug=new QCheckBox(w2,"debug");
  debug->setGeometry(290,30,220,20);
  debug->setText(i18n("generate debugging information"));
  if (cxxflags.contains("-g")) {
    debug->setChecked(true);
  } else {
    debug->setChecked(false);
  }
  KQuickHelp::add(debug, i18n("Checking this turns on the -g flag\n"
														"to generate debugging information."));

  debug_level=new QSpinBox(w2,"debug_level");
  debug_level->setGeometry(310,60,40,20);
  debug_level->setRange(1,3);
  if (cxxflags.contains("-g1")) debug_level->setValue(1);
  if (cxxflags.contains("-g2")) debug_level->setValue(2);
  if (cxxflags.contains("-g3")) debug_level->setValue(3);
  connect( debug_level, SIGNAL(valueChanged(int)),this , SLOT(slotDebug_level_changed(int)) );

  QLabel* debug_level_label;
  debug_level_label=new QLabel(w2,"debug_level_label");
  debug_level_label->setGeometry(370,60,140,20);
  debug_level_label->setText(i18n("Debug-level"));
  KQuickHelp::add(debug_level,
  KQuickHelp::add(debug_level_label, i18n("Set the debugging level here.\n"
					"You can choose from level 1-3 which\n"
					"sets option -g1 to -g3 debugging\n"
					"level.")));


  gprof_info=new QCheckBox(w2,"gprof_info");
  gprof_info->setGeometry(290,90,220,20);
  gprof_info->setText(i18n("generate extra information for gprof"));
  if (cxxflags.contains("-pg")) {
    gprof_info->setChecked(true);
  } else {
    gprof_info->setChecked(false);
  }
  KQuickHelp::add(gprof_info, i18n("Generate extra code to write profile\n"
				"information for the analysis program\n"
				"<i>gprof</i>.")); 

  save_temps=new QCheckBox(w2,"save_temps");
  save_temps->setGeometry(290,120,220,20);
  save_temps->setText(i18n("store temporary intermediate files"));
  if (cxxflags.contains("-save-temps")) {
    save_temps->setChecked(true);
  } else {
    save_temps->setChecked(false);
  }
  KQuickHelp::add(save_temps, i18n("Store the usually temporary intermediate\n"
				"files in the current directory. This means\n"
				"compiling a file <i>foo.c</i> will produce\n"
				"the files <i>foo.o, foo.i</i> and<i>foo.s"));
 
  QLabel* addit_gcc_options_label;
  addit_gcc_options_label=new QLabel(w2,"addit_gcc_options_label");
  addit_gcc_options_label->setGeometry(10,180,200,20);
  addit_gcc_options_label->setText(i18n("additional options:"));

  addit_gcc_options=new QLineEdit(w2,"addit_gcc_options");
  addit_gcc_options->setGeometry(10,210,490,30);
  addit_gcc_options->setText(prj_info->getAdditCXXFLAGS());
  KQuickHelp::add(addit_gcc_options_label,
  KQuickHelp::add(addit_gcc_options, i18n("Insert other GCC-options here\n"
					"to invoke GCC with by setting the\n"
					"CXXFLAGS-environment variable.")));

  addTab(w2,i18n("Compiler Options"));


  // *************** Compiler Warnings *********************

  QWidget *w3= new QWidget(this,"Warnings");
  KQuickHelp::add(w3, i18n("Set the Compiler warnings here by checking\n"
			"the -W options you want to use."));
 
  w_all=new QCheckBox(w3,"w_all");
  w_all->setGeometry(10,10,230,20);
  w_all->setText("-Wall");
  if (cxxflags.contains("-Wall")) {
    w_all->setChecked(true);
  } else {
    w_all->setChecked(false);
  }
  KQuickHelp::add(w_all, i18n("Compile with -Wall. This option\n"
			"includes several different warning\n"
			"parameters which are recommended to\n"
			"turn on."));

  w_=new QCheckBox(w3,"w_");
  w_->setGeometry(10,30,230,20);
  w_->setText("-W");
  if (cxxflags.contains("-W ")) {
    w_->setChecked(true);
  } else {
    w_->setChecked(false);
  }
  KQuickHelp::add(w_, i18n("Compile with -W. This option\n"
			"sets options not included in -Wall\n"
			"which are very specific. Please read\n"
			"GCC-Info for more information."));

  w_traditional=new QCheckBox(w3,"w_traditional");
  w_traditional->setGeometry(10,50,230,20);
  w_traditional->setText("-Wtraditional");
  if (cxxflags.contains("-Wtraditional")) {
    w_traditional->setChecked(true);
  } else {
    w_traditional->setChecked(false);
  }
  KQuickHelp::add(w_traditional, i18n("Warn about certain constructs\n"
				"that behave differently in traditional\n"
				"and ANSI C."));


  w_undef=new QCheckBox(w3,"w_undef");
  w_undef->setGeometry(10,70,230,20);
  w_undef->setText("-Wundef");
  if (cxxflags.contains("-Wundef")) {
    w_undef->setChecked(true);
  } else {
    w_undef->setChecked(false);
  }
  KQuickHelp::add(w_undef, i18n("Warn if an undefined identifier is\n"
				"evaluated in an `#if' directive"));

  w_shadow=new QCheckBox(w3,"w_shadow");
  w_shadow->setGeometry(10,90,230,20);
  w_shadow->setText("-Wshadow");
  if (cxxflags.contains("-Wshadow")) {
    w_shadow->setChecked(true);
  } else {
    w_shadow->setChecked(false);
  }
  KQuickHelp::add(w_shadow, i18n("Warn whenever a local variable\n"
				"shadows another local variable."));

  /*  w_id_clash_len=new QCheckBox(w3,"w_id_clash_len");
  w_id_clash_len->setGeometry(10,110,230,20);
  w_id_clash_len->setText("-Wid_clash-LEN");
  if (cxxflags.contains("-Wid-clash-LEN")) {
    w_id_clash_len->setChecked(true);
  } else {
    w_id_clash_len->setChecked(false);
  }
  KQuickHelp::add(w_id_clash_len, i18n("Warn whenever two distinct\n"
				"identifiers match in the first LEN\n"
				"characters. This may help you prepare\n"
				"a program that will compile with\n"
				"certain obsolete, brain-damaged\n"
				"compilers."));

  w_larger_than_len=new QCheckBox(w3,"w_larger_than_len");
  w_larger_than_len->setGeometry(10,130,230,20);
  w_larger_than_len->setText("-Wlarger-than-LEN");
  if (cxxflags.contains("-Wlarger-than-LEN")) {
    w_larger_than_len->setChecked(true);
  } else {
    w_larger_than_len->setChecked(false);
  }
  KQuickHelp::add(w_larger_than_len, i18n("Warn whenever an object\n"
					"of larger than LEN bytes \n"
					"is defined."));
  */

  w_pointer_arith=new QCheckBox(w3,"w_pointer_arith");
  w_pointer_arith->setGeometry(10,110,230,20);
  w_pointer_arith->setText("-Wpointer-arith");
  if (cxxflags.contains("-Wpointer-arith")) {
    w_pointer_arith->setChecked(true);
  } else {
    w_pointer_arith->setChecked(false);
  }
  KQuickHelp::add(w_pointer_arith, i18n("Warn about anything that\n"
				"depends on the <i>size of</i> a\n"
				"function type or of <i>void</i>.\n"
				"GNU C assigns these types a size of 1,\n"
				"for convenience in calculations with\n"
				"<i>void *</i> pointers and pointers\n"
				"to functions."));


  w_bad_function_cast=new QCheckBox(w3,"w_bad_function_cast");
  w_bad_function_cast->setGeometry(10,130,230,20);
  w_bad_function_cast->setText("-Wbad-function-cast");
  if (cxxflags.contains("-Wbad-function-cast")) {
    w_bad_function_cast->setChecked(true);
  } else {
    w_bad_function_cast->setChecked(false);
  }
  KQuickHelp::add(w_bad_function_cast, i18n("Warn whenever a function call is\n"
					"cast to a non-matching type. For\n"
					"example, warn if <i>int malloc()</i>\n"
					"is cast to <i>anything *."));


  w_cast_qual=new QCheckBox(w3,"w_cast_qual");
  w_cast_qual->setGeometry(10,150,230,20);
  w_cast_qual->setText("-Wcast-qual");
  if (cxxflags.contains("-Wcast-qual")) {
    w_cast_qual->setChecked(true);
  } else {
    w_cast_qual->setChecked(false);
  }
  KQuickHelp::add(w_cast_qual, i18n("Warn whenever a pointer is cast\n"
				"so as to remove a type qualifier\n"
				"from the target type. For example,\n"
				"warn if a <i>const char *</i> is\n"
				"cast to an ordinary <i>char *."));


  w_cast_align=new QCheckBox(w3,"w_cast_align");
  w_cast_align->setGeometry(10,170,230,20);
  w_cast_align->setText("-Wcast-align");
  if (cxxflags.contains("-Wcast-align")) {
    w_cast_align->setChecked(true);
  } else {
    w_cast_align->setChecked(false);
  }
  KQuickHelp::add(w_cast_align, i18n("Warn whenever a pointer is cast such\n"
				"that the required alignment of the target\n"
				"is increased. For example, warn if a\n"
				"<i>char *</i> is cast to an <i>int *</i> on\n"
				"machines where integers can only be accessed\n"
				"at two- or four-byte boundaries."));


  w_write_strings=new QCheckBox(w3,"w_write_strings");
  w_write_strings->setGeometry(10,190,230,20);
  w_write_strings->setText("-Wwrite-strings");
  if (cxxflags.contains("-Wwrite-strings")) {
    w_write_strings->setChecked(true);
  } else {
    w_write_strings->setChecked(false);
  }
  KQuickHelp::add(w_write_strings, 
	i18n("Give string constants the type <i>const char[LENGTH]</i>\n"
	     	"so that copying the address of one into a non-<i>const\n"
	     	"char *</i> pointer will get a warning. These warnings\n"
		"will help you find at compile time code that can try to\n"
		"write into a string constant, but only if you have been\n"
		"very careful about using <i>const</i> in declarations\n"
		"and prototypes. Otherwise, it will just be a nuisance;\n"
		"this is why we did not make <i>-Wall</i> request these\n"
          	"warnings."));


  w_conversion=new QCheckBox(w3,"w_conversion");
  w_conversion->setGeometry(10,210,230,20);
  w_conversion->setText("-Wconversion");
  if (cxxflags.contains("-Wconversion")) {
    w_conversion->setChecked(true);
  } else {
    w_conversion->setChecked(false);
  }
  KQuickHelp::add(w_conversion,
	 i18n("Warn if a prototype causes a type conversion that is different\n"
		"from what would happen to the same argument in the absence\n"
		"of a prototype. This includes conversions of fixed point to\n"
		"floating and vice versa, and conversions changing the width\n"
		"or signedness of a fixed point argument except when the same\n"
		"as the default promotion.  Also warn if a negative integer\n"
		"constant expression is implicitly converted to an unsigned\n"
		"type."));


  w_sign_compare=new QCheckBox(w3,"w_sign_compare");
  w_sign_compare->setGeometry(260,10,230,20);
  w_sign_compare->setText("-Wsign-compare");
  if (cxxflags.contains("-Wsign-compare")) {
    w_sign_compare->setChecked(true);
  } else {
    w_sign_compare->setChecked(false);
  }
  KQuickHelp::add(w_sign_compare,
	i18n("Warn when a comparison between signed and unsigned values\n"
	     "could produce an incorrect result when the signed value\n"
	     "is converted to unsigned."));


  w_aggregate_return=new QCheckBox(w3,"w_aggregate_return");
  w_aggregate_return->setGeometry(260,30,230,20);
  w_aggregate_return->setText("-Waggregate-return");
  if (cxxflags.contains("-Waggregate-return")) {
    w_aggregate_return->setChecked(true);
  } else {
    w_aggregate_return->setChecked(false);
  }
  KQuickHelp::add(w_aggregate_return,
	i18n("Warn if any functions that return structures or unions are\n"
		"defined or called. (In languages where you can return an\n"
		"array, this also elicits a warning.)"));


  w_strict_prototypes=new QCheckBox(w3,"w_strict_prototypes");
  w_strict_prototypes->setGeometry(260,50,230,20);
  w_strict_prototypes->setText("-Wstrict-prototypes");
  if (cxxflags.contains("-Wstrict-prototypes")) {
    w_strict_prototypes->setChecked(true);
  } else {
    w_strict_prototypes->setChecked(false);
  }
  KQuickHelp::add(w_strict_prototypes,
	i18n("Warn if a function is declared or defined without specifying\n"
		"the argument types. (An old-style function definition is\n"
		"permitted without a warning if preceded by a declaration\n"
		"which specifies the argument types.)"));


  w_missing_prototypes=new QCheckBox(w3,"w_missing_prototypes");
  w_missing_prototypes->setGeometry(260,70,230,20);
  w_missing_prototypes->setText("-Wmissing-prototypes");
  if (cxxflags.contains("-Wmissing-prototypes")) {
    w_missing_prototypes->setChecked(true);
  } else {
    w_missing_prototypes->setChecked(false);
  }
  KQuickHelp::add(w_missing_prototypes,
	i18n("Warn if a global function is defined without a previous\n"
		"prototype declaration. This warning is issued even if\n"
		"the definition itself provides a prototype. The aim\n"
		"is to detect global functions that fail to be declared\n"
		"in header files."));


  w_missing_declarations=new QCheckBox(w3,"w_missing_declarations");
  w_missing_declarations->setGeometry(260,90,230,20);
  w_missing_declarations->setText("-Wmissing-declarations");
  if (cxxflags.contains("-Wmissing-declarations")) {
    w_missing_declarations->setChecked(true);
  } else {
    w_missing_declarations->setChecked(false);
  }
  KQuickHelp::add(w_missing_declarations,
	i18n("Warn if a global function is defined without a previous\n"
		"declaration. Do so even if the definition itself pro-\n"
		"vides a prototype. Use this option to detect global\n"
		"functions that are not declared in header files."));


  w_redundant_decls=new QCheckBox(w3,"w_redundant_decls");
  w_redundant_decls->setGeometry(260,110,230,20);
  w_redundant_decls->setText("-Wredundant-decls");
  if (cxxflags.contains("-Wredundant-decls")) {
    w_redundant_decls->setChecked(true);
  } else {
    w_redundant_decls->setChecked(false);
  }
  KQuickHelp::add(w_redundant_decls,
	i18n("Warn if anything is declared more than once in the same scope\n"
		"even in cases where multiple declaration is valid and\n"
		"changes nothing."));


  w_nested_externs=new QCheckBox(w3,"w_nested_externs");
  w_nested_externs->setGeometry(260,130,230,20);
  w_nested_externs->setText("-Wnested-externs");
  if (cxxflags.contains("-Wnested-externs")) {
    w_nested_externs->setChecked(true);
  } else {
    w_nested_externs->setChecked(false);
  }
  KQuickHelp::add(w_nested_externs,
	i18n("Warn if an <i>extern</i> declaration is\n"
		"encountered within a function."));


  w_inline=new QCheckBox(w3,"w_inline");
  w_inline->setGeometry(260,150,230,20);
  w_inline->setText("-Winline");
  if (cxxflags.contains("-Winline")) {
    w_inline->setChecked(true);
  } else {
    w_inline->setChecked(false);
  }
  KQuickHelp::add(w_inline,
	i18n("Warn if a function can not be inlined, and either\n"
		"it was declared as inline, or else the\n"
		"<i>-finline-functions</i> option was given."));


  w_old_style_cast=new QCheckBox(w3,"w_old_style_cast");
  w_old_style_cast->setGeometry(260,170,230,20);
  w_old_style_cast->setText("-Wold-style-cast");
  if (cxxflags.contains("-Wold-style-cast")) {
    w_old_style_cast->setChecked(true);
  } else {
    w_old_style_cast->setChecked(false);
  }
  KQuickHelp::add(w_old_style_cast,
	i18n("Warn if an old-style (C-style) cast is used\n"
	     "within a program"));


  w_overloaded_virtual=new QCheckBox(w3,"w_overloaded_virtual");
  w_overloaded_virtual->setGeometry(260,190,230,20);
  w_overloaded_virtual->setText("-Woverloaded-virtual");
  if (cxxflags.contains("-Woverloaded-virtual")) {
    w_overloaded_virtual->setChecked(true);
  } else {
    w_overloaded_virtual->setChecked(false);
  }
  KQuickHelp::add(w_overloaded_virtual,
	i18n("Warn when a derived class function declaration may be an\n"
		"error in defining a virtual function (C++ only). In\n"
		"a derived class, the definitions of virtual functions\n"
		"must match the type signature of a virtual function\n"
		"declared in the base class. With this option, the\n"
		"compiler warns when you define a function with the same\n"
		"as a virtual function, but with a type signature that\n"
		"does not match any declarations from the base class."));


  w_synth=new QCheckBox(w3,"w_synth");
  w_synth->setGeometry(260,210,230,20);
  w_synth->setText("-Wsynth");
  if (cxxflags.contains("-Wsynth")) {
    w_synth->setChecked(true);
  } else {
    w_synth->setChecked(false);
  }
  KQuickHelp::add(w_synth,
	i18n("Warn when g++'s synthesis behavoir does\n"
		"not match that of cfront."));


  w_error=new QCheckBox(w3,"w_error");
  w_error->setGeometry(260,250,230,20);
  w_error->setText(i18n("make all Warnings into errors"));
  if (cxxflags.contains("-Werror")) {
    w_error->setChecked(true);
  } else {
    w_error->setChecked(false);
  }
  KQuickHelp::add(w_error,
	i18n("Make all warnings into errors."));

  addTab(w3,i18n("Compiler Warnings"));


  // *************** Linker Options *********************

  QWidget *w4= new QWidget(this,"Linker");
  KQuickHelp::add(w4, i18n("Set the Linker options and choose the\n"
			"libraries to add to your project."));
  ldflags = " " + ldflags + " ";
  ldadd = " " + ldadd + " ";
//  KDEBUG1(KDEBUG_INFO,DIALOG,"%s",ldflags.data());
//  KDEBUG1(KDEBUG_INFO,DIALOG,"%s",ldadd.data());
  QGroupBox* ldflags_group;
  ldflags_group=new QGroupBox(w4,"ldflags_group");
  ldflags_group->setGeometry(10,10,530,100);
  ldflags_group->setTitle(i18n("library flags"));
  KQuickHelp::add(ldflags_group, i18n("Set your library flags here."));
 
  l_remove_symbols=new QCheckBox(w4,"l_remove_symbols");
  l_remove_symbols->setGeometry(20,30,470,20);
  l_remove_symbols->setText(i18n("remove all symbol table and relocation information from the executable"));
  if (ldflags.contains(" -s ")) {
    l_remove_symbols->setChecked(true);
    pos=ldflags.find("-s ");
    ldflags.remove(pos,3);
    // cerr << "-s OK" << endl;
  } else {
    l_remove_symbols->setChecked(false);
  }
  KQuickHelp::add(l_remove_symbols, i18n("If you want to use a debugger, you\n"
			"should keep those informations in the object files.\n"
			"It's useless to let the compiler generate debug\n"
			"informations and to remove it with this option."));
  
  l_static=new QCheckBox(w4,"l_static");
  l_static->setGeometry(20,50,360,20);
  l_static->setText(i18n("prevent using shared libraries"));
  if (ldflags.contains(" -static ")) {
    l_static->setChecked(true);
    pos=ldflags.find("-static ");
    ldflags.remove(pos,8);
    // cerr << "-static OK" << endl;
  } else {
    l_static->setChecked(false);
  }
  KQuickHelp::add(l_static, i18n("On systems that support dynamic linking,\n"
				 "this prevents linking with the shared libraries.\n"
				 "On other systems, this option has no effect."));
 
  QLabel* addit_ldflags_label;
  addit_ldflags_label=new QLabel(w4,"addit_ldflags_label");
  addit_ldflags_label->setGeometry(20,70,90,30);
  addit_ldflags_label->setText(i18n("additional flags:"));

  addit_ldflags=new QLineEdit(w4,"addit_ldflags");
  addit_ldflags->setGeometry(120,70,370,30);
  ldflags=ldflags.stripWhiteSpace();
  addit_ldflags->setText(ldflags);
  KQuickHelp::add(addit_ldflags_label,
  KQuickHelp::add(addit_ldflags, i18n("Insert other linker options here\n"
				      "to invoke the linker with by setting the\n"
				      "LDFLAGS-environment variable.")));

  QGroupBox* libs_group;
  libs_group=new QGroupBox(w4,"libs_group");
  libs_group->setGeometry(10,120,530,150);
  libs_group->setTitle(i18n("libraries"));
  KQuickHelp::add(libs_group, i18n("Choose your libraries here."));
 
  l_X11=new QCheckBox(w4,"l_X11");
  l_X11->setGeometry(20,140,110,20);
  l_X11->setText("X11");
  if (ldadd.contains(" -lX11 ")) {
    l_X11->setChecked(true);
    pos=ldadd.find("-lX11 ");
    ldadd.remove(pos,6);
    //cerr << "-lX11 OK" << endl;
  } else {
    l_X11->setChecked(false);
  }
  KQuickHelp::add(l_X11, i18n("X11 basics\n"));

  l_Xext=new QCheckBox(w4,"l_Xext");
  l_Xext->setGeometry(20,160,110,20);
  l_Xext->setText("Xext");
  if (ldadd.contains(" -lXext ")) {
    l_Xext->setChecked(true);
    pos=ldadd.find("-lXext ");
    ldadd.remove(pos,7);
    //cerr << "-lXext OK" << endl;
  } else {
    l_Xext->setChecked(false);
  }
  KQuickHelp::add(l_Xext, i18n("X11 extensions\n"));
  
  l_qt=new QCheckBox(w4,"l_qt");
  l_qt->setGeometry(20,180,110,20);
  l_qt->setText("qt");
  if(!(prj->isKDE2Project() || prj->isQt2Project())){
    if (ldadd.contains(" -lqt ")) {
      l_qt->setChecked(true);
      pos=ldadd.find("-lqt ");
      ldadd.remove(pos,5);
      //cerr << "-lqt OK" << endl;
    }
    else {
      l_qt->setChecked(false);
    }
  }
  else{
    if (ldadd.contains(" $(LIB_QT) ")) {
      l_qt->setChecked(true);
      pos=ldadd.find("$(LIB_QT)");
      ldadd.remove(pos,8);
      //cerr << "-lqt OK" << endl;
    }
    else {
      l_qt->setChecked(false);
    }
  }

  KQuickHelp::add(l_qt, i18n("Qt"));

  l_kdecore=new QCheckBox(w4,"l_kdecore");
  l_kdecore->setGeometry(20,200,110,20);
  l_kdecore->setText("kdecore");
  if (ldadd.contains(" -lkdecore ")) {
    l_kdecore->setChecked(true);
    pos=ldadd.find("-lkdecore ");
    ldadd.remove(pos,10);
    //cerr << "-lkdecore OK" << endl;
  } else {
    l_kdecore->setChecked(false);
  }
  KQuickHelp::add(l_kdecore, i18n("KDE basics"));

  l_kdeui=new QCheckBox(w4,"l_kdeui");
  l_kdeui->setGeometry(140,140,110,20);
  l_kdeui->setText("kdeui");
  if (ldadd.contains(" -lkdeui ")) {
    l_kdeui->setChecked(true);
    pos=ldadd.find("-lkdeui ");
    ldadd.remove(pos,8);
    //cerr << "-lkdeui OK" << endl;
  } else {
    l_kdeui->setChecked(false);
  }
  KQuickHelp::add(l_kdeui, i18n("KDE user interface"));

  l_khtmlw=new QCheckBox(w4,"l_khtmlw");
  l_khtmlw->setGeometry(140,160,110,20);
  l_khtmlw->setText("khtmlw");
  if (ldadd.contains(" -lkhtmlw -lkimgio -ljpeg -ltiff -lpng -lm -ljscript ")) {
    l_khtmlw->setChecked(true);
    pos=ldadd.find("-lkhtmlw -lkimgio -ljpeg -ltiff -lpng -lm -ljscript ");
    ldadd.remove(pos,52);
    //    cerr << "-htmlw OK" << endl;
  } else {
    l_khtmlw->setChecked(false);
  }
  KQuickHelp::add(l_khtmlw, i18n("KDE HTML widget :\n"
				 "this includes -lkhtmlw, -lkimgio, -ljpeg,\n"
				 "-ltiff, -lpng, -lm, -ljscript."));
 
  l_kfm=new QCheckBox(w4,"l_kfm");
  l_kfm->setGeometry(140,180,110,20);
  l_kfm->setText("kfm");
  if (ldadd.contains(" -lkfm ")) {
    l_kfm->setChecked(true);
    pos=ldadd.find("-lkfm ");
    ldadd.remove(pos,6);
    //    cerr << "-lkfm OK" << endl;
  } else {
    l_kfm->setChecked(false);
  }
  KQuickHelp::add(l_kfm, i18n("KDE kfm functionality"));
 
  l_kfile=new QCheckBox(w4,"l_kfile");
  l_kfile->setGeometry(140,200,110,20);
  l_kfile->setText("kfile");
  if (ldadd.contains(" -lkfile ")) {
    l_kfile->setChecked(true);
    pos=ldadd.find("-lkfile ");
    ldadd.remove(pos,8);
    //    cerr << "-lkfile OK" << endl;
  } else {
    l_kfile->setChecked(false);
  }
  KQuickHelp::add(l_kfile, i18n("KDE file handling"));

  l_kspell=new QCheckBox(w4,"l_kspell");
  l_kspell->setGeometry(260,140,110,20);
  l_kspell->setText("kspell");
  if (ldadd.contains(" -lkspell ")) {
    l_kspell->setChecked(true);
    pos=ldadd.find("-lkspell ");
    ldadd.remove(pos,9);
    //    cerr << "-lkspell OK" << endl;
  } else {
    l_kspell->setChecked(false);
  }
  KQuickHelp::add(l_kspell, i18n("KDE Spell checking"));
 
  l_kab=new QCheckBox(w4,"l_kab");
  l_kab->setGeometry(260,160,110,20);
  l_kab->setText("kab");
  if (ldadd.contains(" -lkab ")) {
    l_kab->setChecked(true);
    pos=ldadd.find("-lkab ");
    ldadd.remove(pos,6);
    //    cerr << "-lkab OK" << endl;
  } else {
    l_kab->setChecked(false);
  }
  KQuickHelp::add(l_kab, i18n("KDE addressbook"));
 
  l_math=new QCheckBox(w4,"l_math");
  l_math->setGeometry(380,140,105,20);
  l_math->setText("math");
  if (l_khtmlw->isChecked() || ldadd.contains(" -lm ")) {
    l_math->setChecked(true);
    pos=ldadd.find("-lm ");
    if (pos>=0)
     ldadd.remove(pos,4);
    //    cerr << "-lm OK" << endl;
  } else {
    l_math->setChecked(false);
  }
  KQuickHelp::add(l_math, i18n("Math library"));

  QLabel* addit_ldadd_label;
  addit_ldadd_label=new QLabel(w4,"addit_ldadd_label");
  addit_ldadd_label->setGeometry(20,230,110,30);
  addit_ldadd_label->setText(i18n("additional libraries:"));

  addit_ldadd=new QLineEdit(w4,"addit_ldadd");
  addit_ldadd->setGeometry(140,230,350,30);
  ldadd=ldadd.stripWhiteSpace();

  addit_ldadd->setText(ldadd);
  KQuickHelp::add(addit_ldadd_label,
  KQuickHelp::add(addit_ldadd, i18n("Add additional libraries here.")));

  addTab(w4,i18n("Linker Options"));

  // ****************** the Make_Widget ********************
  QWidget *w5 = new QWidget( this, "make" );
  KQuickHelp::add(w5, i18n("This dialog is for setting\nyour make options."));

  m_print_debug_info = new QCheckBox( w5, "m_print_debug_info" );
  m_print_debug_info->setGeometry( 10, 20, 190, 25 );
  m_print_debug_info->setFocusPolicy( QWidget::TabFocus );
  m_print_debug_info->setBackgroundMode( QWidget::PaletteBackground );
  m_print_debug_info->setFontPropagation( QWidget::NoChildren );
  m_print_debug_info->setPalettePropagation( QWidget::NoChildren );
  m_print_debug_info->setText(i18n("Print debug information"));
  m_print_debug_info->setAutoRepeat( FALSE );
  m_print_debug_info->setAutoResize( FALSE );

  m_cont_after_error = new QCheckBox( w5, "m_cont_after_error" );
  m_cont_after_error->setGeometry( 200, 20, 170, 25 );
  m_cont_after_error->setFocusPolicy( QWidget::TabFocus );
  m_cont_after_error->setBackgroundMode( QWidget::PaletteBackground );
  m_cont_after_error->setFontPropagation( QWidget::NoChildren );
  m_cont_after_error->setPalettePropagation( QWidget::NoChildren );
  m_cont_after_error->setText(i18n("Continue after errors"));
  m_cont_after_error->setAutoRepeat( FALSE );
  m_cont_after_error->setAutoResize( FALSE );

  m_print_data_base = new QCheckBox( w5, "m_print_data_base" );
  m_print_data_base->setGeometry( 375, 20, 170, 25 );
  m_print_data_base->setFocusPolicy( QWidget::TabFocus );
  m_print_data_base->setBackgroundMode( QWidget::PaletteBackground );
  m_print_data_base->setFontPropagation( QWidget::NoChildren );
  m_print_data_base->setPalettePropagation( QWidget::NoChildren );
  m_print_data_base->setText(i18n("Print the data base"));
  m_print_data_base->setAutoRepeat( FALSE );
  m_print_data_base->setAutoResize( FALSE );

  m_env_variables = new QCheckBox( w5, "m_env_variables" );
  m_env_variables->setGeometry( 10, 50, 190, 25 );
  m_env_variables->setFocusPolicy( QWidget::TabFocus );
  m_env_variables->setBackgroundMode( QWidget::PaletteBackground );
  m_env_variables->setFontPropagation( QWidget::NoChildren );
  m_env_variables->setPalettePropagation( QWidget::NoChildren );
  m_env_variables->setText(i18n("Environment variables"));
  m_env_variables->setAutoRepeat( FALSE );
  m_env_variables->setAutoResize( FALSE );

  m_no_rules = new QCheckBox( w5, "m_no_rules" );
  m_no_rules->setGeometry( 200, 50, 170, 25 );
  m_no_rules->setFocusPolicy( QWidget::TabFocus );
  m_no_rules->setBackgroundMode( QWidget::PaletteBackground );
  m_no_rules->setFontPropagation( QWidget::NoChildren );
  m_no_rules->setPalettePropagation( QWidget::NoChildren );
  m_no_rules->setText(i18n("No built-in rules"));
  m_no_rules->setAutoRepeat( FALSE );
  m_no_rules->setAutoResize( FALSE );

  m_touch_files = new QCheckBox( w5, "m_touch_files" );
  m_touch_files->setGeometry( 375, 50, 170, 25 );
  m_touch_files->setFocusPolicy( QWidget::TabFocus );
  m_touch_files->setBackgroundMode( QWidget::PaletteBackground );
  m_touch_files->setFontPropagation( QWidget::NoChildren );
  m_touch_files->setPalettePropagation( QWidget::NoChildren );
  m_touch_files->setText(i18n("Touch files"));
  m_touch_files->setAutoRepeat( FALSE );
  m_touch_files->setAutoResize( FALSE );

  m_ignor_errors = new QCheckBox( w5, "m_ignor_errors" );
  m_ignor_errors->setGeometry( 10, 80, 190, 25 );
  m_ignor_errors->setFocusPolicy( QWidget::TabFocus );
  m_ignor_errors->setBackgroundMode( QWidget::PaletteBackground );
  m_ignor_errors->setFontPropagation( QWidget::NoChildren );
  m_ignor_errors->setPalettePropagation( QWidget::NoChildren );
  m_ignor_errors->setText(i18n("Ignore all errors"));
  m_ignor_errors->setAutoRepeat( FALSE );
  m_ignor_errors->setAutoResize( FALSE );

  m_silent_operation = new QCheckBox( w5, "m_silent_operation" );
  m_silent_operation->setGeometry( 200, 80, 170, 25 );
  m_silent_operation->setFocusPolicy( QWidget::TabFocus );
  m_silent_operation->setBackgroundMode( QWidget::PaletteBackground );
  m_silent_operation->setFontPropagation( QWidget::NoChildren );
  m_silent_operation->setPalettePropagation( QWidget::NoChildren );
  m_silent_operation->setText(i18n("Silent operation"));
  m_silent_operation->setAutoRepeat( FALSE );
  m_silent_operation->setAutoResize( FALSE );

  m_print_work_dir = new QCheckBox( w5, "m_print_work_dir" );
  m_print_work_dir->setGeometry( 375, 80, 170, 25 );
  m_print_work_dir->setFocusPolicy( QWidget::TabFocus );
  m_print_work_dir->setBackgroundMode( QWidget::PaletteBackground );
  m_print_work_dir->setFontPropagation( QWidget::NoChildren );
  m_print_work_dir->setPalettePropagation( QWidget::NoChildren );
  m_print_work_dir->setText(i18n("Print working directory"));
  m_print_work_dir->setAutoRepeat( FALSE );
  m_print_work_dir->setAutoResize( FALSE );

  m_job_number_label = new QLabel( w5, "m_job_number_label" );
  m_job_number_label->setGeometry( 10, 110, 100, 25 );
  m_job_number_label->setFocusPolicy( QWidget::NoFocus );
  m_job_number_label->setBackgroundMode( QWidget::PaletteBackground );
  m_job_number_label->setFontPropagation( QWidget::NoChildren );
  m_job_number_label->setPalettePropagation( QWidget::NoChildren );
  m_job_number_label->setText(i18n("job number"));
  m_job_number_label->setAlignment( 289 );
  m_job_number_label->setMargin( -1 );

  m_job_number = new QSpinBox( w5, "m_job_number" );
  m_job_number->setGeometry( 120, 110, 60, 25 );
  m_job_number->setFocusPolicy( QWidget::StrongFocus );
  m_job_number->setBackgroundMode( QWidget::PaletteBackground );
  m_job_number->setFontPropagation( QWidget::NoChildren );
  m_job_number->setPalettePropagation( QWidget::NoChildren );
  m_job_number->setFrameStyle( 50 );
  m_job_number->setLineWidth( 2 );
  m_job_number->setRange( 1, 99 );
  m_job_number->setSteps( 1, 0 );
  m_job_number->setPrefix( "" );
  m_job_number->setSuffix( "" );
  m_job_number->setSpecialValueText( "" );
  m_job_number->setWrapping( FALSE );

  m_set_modify_label = new QLabel( w5, "m_set_modify_label" );
  m_set_modify_label->setGeometry( 10, 150, 100, 25 );
  m_set_modify_label->setFocusPolicy( QWidget::NoFocus );
  m_set_modify_label->setBackgroundMode( QWidget::PaletteBackground );
  m_set_modify_label->setFontPropagation( QWidget::NoChildren );
  m_set_modify_label->setPalettePropagation( QWidget::NoChildren );
  m_set_modify_label->setText(i18n("set modified"));
  m_set_modify_label->setAlignment( 289 );
  m_set_modify_label->setMargin( -1 );

  m_set_modify_line = new QLineEdit( w5, "m_set_modify_line" );
  m_set_modify_line->setGeometry( 120, 150, 330, 25 );
  m_set_modify_line->setMinimumSize( 0, 0 );
  m_set_modify_line->setMaximumSize( 32767, 32767 );
  m_set_modify_line->setFocusPolicy( QWidget::StrongFocus );
  m_set_modify_line->setBackgroundMode( QWidget::PaletteBase );
  m_set_modify_line->setFontPropagation( QWidget::NoChildren );
  m_set_modify_line->setPalettePropagation( QWidget::NoChildren );
  m_set_modify_line->setText("");
  m_set_modify_line->setMaxLength( 32767 );
  m_set_modify_line->setEchoMode( QLineEdit::Normal );
  m_set_modify_line->setFrame( TRUE );

  m_set_modify_dir = new QPushButton( w5, "m_set_modify_dir" );
  m_set_modify_dir->setGeometry( 460, 150, 30, 25 );
  m_set_modify_dir->setMinimumSize( 0, 0 );
  m_set_modify_dir->setMaximumSize( 32767, 32767 );
  m_set_modify_dir->setFocusPolicy( QWidget::TabFocus );
  m_set_modify_dir->setBackgroundMode( QWidget::PaletteBackground );
  m_set_modify_dir->setFontPropagation( QWidget::NoChildren );
  m_set_modify_dir->setPalettePropagation( QWidget::NoChildren );
	QPixmap pix;
  pix.load(KApplication::kde_datadir() + "/kdevelop/toolbar/open.xpm");
  m_set_modify_dir->setPixmap(pix);
  m_set_modify_dir->setAutoRepeat( FALSE );
  m_set_modify_dir->setAutoResize( FALSE );
  
  m_optional_label = new QLabel( w5, "m_optional_label" );
  m_optional_label->setGeometry( 10, 190, 100, 25 );
  m_optional_label->setFocusPolicy( QWidget::NoFocus );
  m_optional_label->setBackgroundMode( QWidget::PaletteBackground );
  m_optional_label->setFontPropagation( QWidget::NoChildren );
  m_optional_label->setPalettePropagation( QWidget::NoChildren );
  m_optional_label->setText(i18n("additional options"));
  m_optional_label->setAlignment( 289 );
  m_optional_label->setMargin( -1 );

  m_optional_line = new QLineEdit( w5, "m_optional_line" );
  m_optional_line->setGeometry( 120, 190, 370, 25 );
  m_optional_line->setMinimumSize( 0, 0 );
  m_optional_line->setMaximumSize( 32767, 32767 );
  m_optional_line->setFocusPolicy( QWidget::StrongFocus );
  m_optional_line->setBackgroundMode( QWidget::PaletteBase );
  m_optional_line->setFontPropagation( QWidget::NoChildren );
  m_optional_line->setPalettePropagation( QWidget::NoChildren );
  m_optional_line->setText(i18n(""));
  m_optional_line->setMaxLength( 32767 );
  m_optional_line->setEchoMode( QLineEdit::Normal );
  m_optional_line->setFrame( TRUE );

  m_rebuild_label = new QLabel( w5, "m_rebuild_label" );
  m_rebuild_label->setGeometry( 10, 235, 250, 25 );
  m_rebuild_label->setFocusPolicy( QWidget::NoFocus );
  m_rebuild_label->setBackgroundMode( QWidget::PaletteBackground );
  m_rebuild_label->setFontPropagation( QWidget::NoChildren );
  m_rebuild_label->setPalettePropagation( QWidget::NoChildren );
  m_rebuild_label->setText(i18n("Rebuild behaviour on run/debug:"));
  m_rebuild_label->setAlignment( 289 );
  m_rebuild_label->setMargin( -1 );

  m_rebuild_combo = new QComboBox( w5, "m_rebuild_label" );
  m_rebuild_combo->setGeometry( 270, 235, 220, 25 );
  m_rebuild_combo->setFocusPolicy( QWidget::NoFocus );
  m_rebuild_combo->setBackgroundMode( QWidget::PaletteBase );
  m_rebuild_combo->setFontPropagation( QWidget::NoChildren );
  m_rebuild_combo->setPalettePropagation( QWidget::NoChildren );
  m_rebuild_combo->insertItem(i18n("warning on modification"));
  m_rebuild_combo->insertItem(i18n("only on modification"));
  m_rebuild_combo->insertItem(i18n("always rebuild"));

  addTab(w5,i18n("Make Options"));

  KQuickHelp::add(m_set_modify_dir, i18n("Pressing the folder button lets you choose\n"
  					 "a file which will be set modified. This will\n"
  					 "recompile the file on the next build run."));

  KQuickHelp::add(m_print_debug_info, i18n("Print  debugging  information  in  addition to normal\n"
					   "processing. The debugging information tells you which\n"
					   "files  are being considered for remaking, which file-\n"
					   "times are being compared and with what results, which\n"
					   "files  actually  need  to  be  remade, which implicit\n"
					   "rules are considered and which  are  applied---every�\n"
					   "thing  interesting about how make decides what to do."));

  KQuickHelp::add(m_optional_label,
  KQuickHelp::add(m_optional_line, i18n("Set any other additional options for your\n"
  					"make-program here.")));

  KQuickHelp::add(m_rebuild_label,
  KQuickHelp::add(m_rebuild_combo, i18n("Set the rebuild type on pressing run/debug here. This allows you to select if you either\n"
  					"want to rebuild always on run/debug or rebuild only on modifications or\n"
					"give you first a warning on modification of your sources.")));

  KQuickHelp::add(m_print_data_base, i18n("Print  the data base (rules and variable values) that\n"
					  "results from reading the makefiles; then  execute  as\n"
					  "usual  or  as  otherwise specified.  This also prints\n"
					  "the version information."));

  KQuickHelp::add(m_no_rules, i18n("Eliminate  use  of the built-in implicit rules.  Also\n"
				   "clear out the default list  of  suffixes  for  suffix\n"
				   "rules."));

  KQuickHelp::add(m_env_variables, i18n("Give variables taken from the environment  precedence\n"
					"over variables from makefiles."));

  KQuickHelp::add(m_cont_after_error, i18n("Continue  as  much as possible after an error. While\n"
					   "the target that failed, and those that depend on it,\n"
					   "cannot  be  remade, the  other dependencies of these\n"
					   "targets can be processed all the same."));

  KQuickHelp::add(m_touch_files, i18n("Touch  files  (mark  them  up  to date without really\n"
				      "changing them) instead  of  running  their  commands.\n"
				      "This  is used to pretend that the commands were done,\n"
				      "in order to fool future invocations of make."));

  KQuickHelp::add(m_print_work_dir, i18n("Print  a  message  containing  the  working directory\n"
					 "before and after other processing.  This may be  use�\n"
					 "ful  for  tracking down errors from complicated nests\n"
					 "of recursive make commands."));

  KQuickHelp::add(m_silent_operation, i18n("Silent operation; do not print the commands as they\n"
					   "are executed."));

  KQuickHelp::add(m_ignor_errors, i18n("Ignore all errors in commands executed to\n"
				       "remake files."));

  KQuickHelp::add(m_job_number_label,
  KQuickHelp::add(m_job_number, i18n("Specifies the number of jobs (commands) to run\n"
					   "simultaneously. If you have a single CPU-System\n"
					   "it is not recommended to choose more then 2.")));

  KQuickHelp::add(m_set_modify_label, i18n("Pretend that the target file has just been  modified.\n"
					   "It  is  almost the same as running a touch command on\n"
					   "the given file before running make, except  that  the\n"
					   "modification  time is changed only in the imagination\n"
					   "of make."));

  connect(m_set_modify_dir,SIGNAL(clicked()),SLOT(slotFileDialogClicked()));

  settings = kapp->getConfig();
  settings->setGroup("MakeOptionsSettings");

  if (settings->readBoolEntry("PrintDebugInfo")) {
  	m_print_debug_info->setChecked(TRUE);
  }
 	else m_print_debug_info->setChecked(FALSE);

 	if (settings->readBoolEntry("PrintDataBase")) {
 	  m_print_data_base->setChecked(TRUE);
  }
  else m_print_data_base->setChecked(FALSE);

  if (settings->readBoolEntry("NoRules")) {
  	m_no_rules->setChecked(TRUE);
  }
  else m_no_rules->setChecked(FALSE);

  if (settings->readBoolEntry("EnvVariables")) {
  	m_env_variables->setChecked(TRUE);
  }
  else m_env_variables->setChecked(FALSE);

  if (settings->readBoolEntry("ContAfterError")) {
  	m_cont_after_error->setChecked(TRUE);
	}
	else m_cont_after_error->setChecked(FALSE);

	if (settings->readBoolEntry("TouchFiles")) {
		m_touch_files->setChecked(TRUE);
  }
	else m_touch_files->setChecked(FALSE);

	if (settings->readBoolEntry("PrintWorkDir")){
		m_print_work_dir->setChecked(TRUE);
	}
	else m_print_work_dir->setChecked(FALSE);

	if (settings->readBoolEntry("SilentOperation")) {
		m_silent_operation->setChecked(TRUE);
	}
	else m_silent_operation->setChecked(FALSE);

	if (settings->readBoolEntry("IgnorErrors")) {
		m_ignor_errors->setChecked(TRUE);
	}
	else m_ignor_errors->setChecked(FALSE);

	m_set_modify_line->setText(settings->readEntry("SetModifyLine"));
	m_optional_line->setText(settings->readEntry("OptionalLine"));
	m_job_number->setValue(settings->readNumEntry ("JobNumber"));

	m_rebuild_combo->setCurrentItem(settings->readNumEntry("RebuildType", 2));

//************************** binary selection *************************//
  QWidget* w6 = new QWidget( this, "bin" );

  QGroupBox* binary_box= new QGroupBox(w6,"binary_box");
  binary_box->setGeometry(10,10,560,150);
  binary_box->setMinimumSize(0,0);
  binary_box->setTitle(i18n("Name"));

  QLabel* binary = new QLabel(w6,"binary_label");
  binary->setGeometry(30,40,500,30);
  binary->setMinimumSize(0,0);
  binary->setText(i18n("Filename of binary that will be started on \"Execute\":"));

  binary_edit= new QLineEdit(w6,"binary_edit");
  binary_edit->setGeometry(30,70,470,30);
  binary_edit->setMinimumSize(0,0);
  binary_edit->setMaxLength( 32767 );

  QString binaryFileName;
  binaryFileName = prj->getBinPROGRAM();
  if (!binaryFileName.isEmpty()){
    binary_edit->setText(binaryFileName);
  }

  QPushButton* binary_button= new QPushButton(w6,"binary_button");
  binary_button->setGeometry(510,70,30,30);
  binary_button->setMinimumSize(0,0);
  binary_button->setPixmap(pix);

  KQuickHelp::add(binary_edit,
  KQuickHelp::add(binary_button,
  KQuickHelp::add(binary,i18n("Set the directory path leading to your binary that is expected to be started on Run.\n"
                              "This is useful in projects for a dynamical library!\n"
			      "Hint: Use a relative path starting from your source directory to be location independent."))));


  connect(binary_button,SIGNAL(clicked()),SLOT(slotBinaryClicked()));
  addTab(w6, i18n("Binary"));


  // **************set the button*********************
  setOkButton(i18n("OK"));
  setCancelButton(i18n("Cancel"));
  connect( this, SIGNAL(applyButtonPressed()), SLOT(ok()) );

  // **************set tabs enlabe/disable*********************

  if(prj->isCustomProject()){
    setTabEnabled("Compiler options",false);
    setTabEnabled("Warnings",false);
    setTabEnabled("Linker",false);
  }
 
}

void CPrjOptionsDlg::slotOptimize_level_changed(int v) {

    if (v>3) {
	optimize_level->setValue(3);
    }
    if (v<1) {
	optimize_level->setValue(1);
    }

}

void CPrjOptionsDlg::slotDebug_level_changed(int v) {

    if (v>3) {
	debug_level->setValue(3);
    }
    if (v<1) {
	debug_level->setValue(1);
    }

}

void CPrjOptionsDlg::ok(){

  QString text,text2;
  QStrList short_info;
  int i,n;


  if (optimize_level->value()>3) {
      optimize_level->setValue(3);
  }
  if (optimize_level->value()<1) {
      optimize_level->setValue(1);
  }
  if (debug_level->value()>3) {
      debug_level->setValue(3);
  }
  if (debug_level->value()<1) {
      debug_level->setValue(1);
  }


  //*********general******************
  text = prjname_edit->text();
  prj_info->setProjectName(text);
  text = author_edit->text();
  prj_info->setAuthor(text);
  text = email_edit->text();
  prj_info->setEmail(text);
  text = version_edit->text();
  prj_info->setVersion(text);
  n=info_edit->numLines();
  for (i=0;(i!=n);i++) {
    text=info_edit->textLine(i);
    short_info.append(text);
  }
  prj_info->setShortInfo(short_info);
  text="";

  QString vcsystem = vcsystem_combo->currentText();
  prj_info->setVCSystem(vcsystem == i18n("None")? QString("None") : vcsystem);

  prj_info->setModifyMakefiles(modifymakefiles_checkbox->isChecked());

  //********gcc-options***************
  if (target->currentItem()) {
    text=" -b "+QString(target->currentText());
  }
  // if (syntax_check->isChecked()) {
//     text+=" -fsyntax-only";
//   }
  if (optimize->isChecked()) {
    text2.setNum(optimize_level->value());
    text+=" -O"+text2;
  } else {
    text+=" -O0";
  }
  if (debug->isChecked()) {
    text2.setNum(debug_level->value());
    text+=" -g"+text2;
  }
  if (gprof_info->isChecked()) {
    text+=" -pg";
  }
  if (save_temps->isChecked()) {
    text+=" -save-temps";
  }
  prj_info->setAdditCXXFLAGS(addit_gcc_options->text());
  if(old_addit_flags != prj_info->getAdditCXXFLAGS().stripWhiteSpace()){
    need_makefile_generation = true;
  }
  //***********gcc-warnings***********
  if (w_all->isChecked()) {
    text+=" -Wall";
  }
  if (w_->isChecked()) {
    text+=" -W ";
  }
  if (w_traditional->isChecked()) {
    text+=" -Wtraditional";
  }
  if (w_undef->isChecked()) {
    text+=" -Wundef";
  }
  if (w_shadow->isChecked()) {
    text+=" -Wshadow";
  }
  //  if (w_id_clash_len->isChecked()) {
  //    text+=" -Wid-clash-LEN";
  //  }
  //  if (w_larger_than_len->isChecked()) {
  //    text+=" -Wlarger-than-LEN";
  //  }
  if (w_pointer_arith->isChecked()) {
    text+=" -Wpointer-arith";
  }
  if (w_bad_function_cast->isChecked()) {
    text+=" -Wbad-function-cast";
  }
  if (w_cast_qual->isChecked()) {
    text+=" -Wcast-qual";
  }
  if (w_cast_align->isChecked()) {
    text+=" -Wcast-align";
  }
  if (w_write_strings->isChecked()) {
    text+=" -Wwrite-strings";
  }
  if (w_conversion->isChecked()) {
    text+=" -Wconversion";
  }
  if (w_sign_compare->isChecked()) {
    text+=" -Wsign-compare";
  }
  if (w_aggregate_return->isChecked()) {
    text+=" -Waggregate-return";
  }
  if (w_strict_prototypes->isChecked()) {
    text+=" -Wstrict-prototypes";
  }
  if (w_missing_prototypes->isChecked()) {
    text+=" -Wmissing-prototypes";
  }
  if (w_missing_declarations->isChecked()) {
    text+=" -Wmissing-declarations";
  }
  if (w_redundant_decls->isChecked()) {
    text+=" -Wredundant-decls";
  }
  if (w_nested_externs->isChecked()) {
    text+=" -Wnested-externs";
  }
  if (w_inline->isChecked()) {
    text+=" -Winline";
  }
  if (w_old_style_cast->isChecked()) {
    text+=" -Wold-style-cast";
  }
  if (w_overloaded_virtual->isChecked()) {
    text+=" -Woverloaded-virtual";
  }
  if (w_synth->isChecked()) {
    text+=" -Wsynth";
  }
  if (w_error->isChecked()) {
    text+=" -Werror";
  }
  prj_info->setCXXFLAGS(text);
  if(old_cxxflags !=  prj_info->getCXXFLAGS().stripWhiteSpace()){
    need_makefile_generation = true;
  }
  //**********linker options*************
  text=addit_ldflags->text();

  if (l_remove_symbols->isChecked()) {
    text+=" -s ";
  }
  if (l_static->isChecked()) {
    text+=" -static";
  }
  prj_info->setLDFLAGS(text);
  if(old_ldflags != prj_info->getLDFLAGS().stripWhiteSpace()){
    need_makefile_generation = true;
  }

  text= addit_ldadd->text();

  if (l_math->isChecked() && !l_khtmlw->isChecked()) {
    text+=" -lm";
  }
  if (l_kab->isChecked()) {
    text+=" -lkab";
  }
  if (l_kspell->isChecked()) {
    text+=" -lkspell";
  }
  if (l_kfile->isChecked()) {
    text+=" -lkfile";
  }
  if (l_kfm->isChecked()) {
    text+=" -lkfm";
  }
  if (l_khtmlw->isChecked()) {
    text+=" -lkhtmlw -lkimgio -ljpeg -ltiff -lpng -lm -ljscript";
  }
  if (l_kdeui->isChecked()) {
    text+=" -lkdeui";
  }
  if (l_kdecore->isChecked()) {
    text+=" -lkdecore";
  }
  if (l_qt->isChecked()) {
   if(!(prj_info->isKDE2Project() || prj_info->isQt2Project()))
    text+=" -lqt";
   else
    text+=" $(LIB_QT)";
  }
  if (l_Xext->isChecked()) {
    text+=" -lXext";
  }
  if (l_X11->isChecked()) {
    text+=" -lX11";
  }
  prj_info->setLDADD(text);
  if(old_ldadd != prj_info->getLDADD().stripWhiteSpace()){
    need_makefile_generation = true;
  }

  //**********make options*************
  settings = kapp->getConfig();
  settings->setGroup("MakeOptionsSettings");

  if (m_print_debug_info->isChecked()) {
    settings->writeEntry("PrintDebugInfo",TRUE);
  }
  else settings->writeEntry("PrintDebugInfo",FALSE);

  if (m_print_data_base->isChecked()) {
    settings->writeEntry("PrintDataBase",TRUE);
  }
  else settings->writeEntry("PrintDataBase",FALSE);

  if (m_no_rules->isChecked()) {
    settings->writeEntry("NoRules",TRUE);
  }
  else settings->writeEntry("NoRules",FALSE);

  if (m_env_variables->isChecked()) {
    settings->writeEntry("EnvVariables",TRUE);
  }
  else settings->writeEntry("EnvVariables",FALSE);

  if (m_cont_after_error->isChecked()) {
		settings->writeEntry("ContAfterError",TRUE);
	}
	else settings->writeEntry("ContAfterError",FALSE);

	if (m_touch_files->isChecked()) {
		settings->writeEntry("TouchFiles", TRUE);
  }
  else settings->writeEntry("TouchFiles", FALSE);

	if (m_print_work_dir->isChecked()){
  	settings->writeEntry("PrintWorkDir", TRUE);
	}
	else settings->writeEntry("PrintWorkDir", FALSE);

	if (m_silent_operation->isChecked()) {
		settings->writeEntry("SilentOperation", TRUE);
	}
	else settings->writeEntry("SilentOperation", FALSE);

	if (m_ignor_errors->isChecked()) {
		settings->writeEntry("IgnorErrors", TRUE);
	}
	else settings->writeEntry("IgnorErrors", FALSE);

	settings->writeEntry("RebuildType", m_rebuild_combo->currentItem());
	settings->writeEntry("SetModifyLine", m_set_modify_line->text());
	settings->writeEntry("OptionalLine", m_optional_line->text());
	settings->writeEntry("JobNumber", m_job_number->text());	
  settings->sync();
  // reject();	

  text = "";

  if (m_print_debug_info->isChecked()) {
    text+=" -d";
  }
  if (m_print_data_base->isChecked()) {
    text+=" -p";
  }

  if (m_no_rules->isChecked()) {
    text+=" -r";
  }
  if (m_env_variables->isChecked()) {
    text+=" -e";
  }
  if (m_cont_after_error->isChecked()) {
    text+=" -k";
  }
  if (m_touch_files->isChecked()) {
    text+=" -t";
  }
  if (m_print_work_dir->isChecked()) {
    text+=" -w";
  }
  if (m_silent_operation->isChecked()) {
    text+=" -s";
  }
  if (m_ignor_errors->isChecked()) {
    text+=" -i";
  }

  text+=" -j";
  text+= m_job_number->text();

  QString *str1 = new QString(m_set_modify_line->text());
  if (!str1->isEmpty()) {
    text+=" -W";
    text+= m_set_modify_line->text();
  }
  delete (str1);

  QString *str2 = new QString(m_optional_line->text());
  if (!str2->isEmpty()) {
    text+=" ";
    text+= m_optional_line->text();
  }
  delete (str2);

  prj_info->setMakeOptions (text);

  /******** binary options *********/
  prj_info->setBinPROGRAM( binary_edit->text());

  // write it to the disk
  prj_info->writeProject();
  if (version_edit->text() != old_version){
    need_configure_in_update = true;
  }
}

// connection to set_modify_dir
void CPrjOptionsDlg::slotFileDialogClicked() {
  QString file,dir;
  dir = prj_info->getProjectDir();
  file = filedialog->getOpenFileName(dir,"*",this,"File");
  m_set_modify_line->setText(file);
}


bool CPrjOptionsDlg::needConfigureInUpdate(){
  return  need_configure_in_update;
}


void CPrjOptionsDlg::slotBinaryClicked(){
  QString dir;
  dir = KFileDialog::getOpenFileName(prj_info->getBinPROGRAM());
  if (!dir.isEmpty()){
    binary_edit->setText(dir);
  }
}
