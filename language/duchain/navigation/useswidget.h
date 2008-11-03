/*
   Copyright 2008 David Nolden <david.nolden.kdevelop@art-master.de>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License version 2 as published by the Free Software Foundation.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/
#ifndef USESWIDGET_H
#define USESWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QScrollArea>

#include <language/duchain/declaration.h>
#include <language/duchain/topducontext.h>
#include <editor/simplecursor.h>
#include "../../languageexport.h"
#include "usescollector.h"

class KComboBox;
class QComboBox;
class QToolButton;
class QVBoxLayout;
class QHBoxLayout;
class QBoxLayout;
class CodeRepresentation;
class QPushButton;
class QProgressBar;

namespace KDevelop {
    class IndexedDeclaration;
    ///A widget representing one use of a Declaration in a speicific context
    class KDEVPLATFORMLANGUAGE_EXPORT OneUseWidget : public QLabel, public KTextEditor::SmartRangeWatcher {
      Q_OBJECT
      public:
        OneUseWidget(IndexedDeclaration declaration, IndexedString document, SimpleRange range, const CodeRepresentation& code, KTextEditor::SmartRange* smartRange);
        ~OneUseWidget();
        
      private slots:
        void jumpTo();
      private:
        virtual void resizeEvent ( QResizeEvent * event );
        virtual void rangeDeleted(KTextEditor::SmartRange* range);
        
        SimpleRange m_range;
        KTextEditor::SmartRange* m_smartRange;
        IndexedDeclaration m_declaration;
        IndexedString m_document;
        QString m_sourceLine;
    };
    
    
    class KDEVPLATFORMLANGUAGE_EXPORT NavigatableWidgetList : public QScrollArea {
      Q_OBJECT
      public:
        NavigatableWidgetList(bool allowScrolling = false, uint maxHeight = 0, bool vertical = true);
        ~NavigatableWidgetList();
        void addItem(QWidget* widget, int pos = -1);
        void addHeaderItem(QWidget* widget, Qt::Alignment alignment = 0);
        ///Whether items were added to this list using addItem(..)
        bool hasItems() const;
        ///Deletes all items that were added using addItem
        void deleteItems();
        QList<QWidget*> items() const;
        void setShowHeader(bool show);
      private:
        QVBoxLayout* m_layout;
        QBoxLayout* m_itemLayout;
        QHBoxLayout* m_headerLayout;
        QToolButton *m_previousButton, *m_nextButton;
        uint m_maxHeight;
        bool m_allowScrolling, m_useArrows;
    };
    
    class KDEVPLATFORMLANGUAGE_EXPORT ContextUsesWidget : public NavigatableWidgetList {
      Q_OBJECT
      public:
        ContextUsesWidget(const CodeRepresentation& code, IndexedDeclaration usedDeclaration, IndexedDUContext context);
      Q_SIGNALS:
        void navigateDeclaration(KDevelop::IndexedDeclaration);
      private Q_SLOTS:
        void linkWasActivated(QString);
      private:
        IndexedDeclaration m_usedDeclaration;
        IndexedDUContext m_context;
    };
    
    /**
     * Represents the uses of a declaration within one top-context
     */
    class KDEVPLATFORMLANGUAGE_EXPORT TopContextUsesWidget : public NavigatableWidgetList {
        Q_OBJECT
        public:
            TopContextUsesWidget(IndexedDeclaration declaration, IndexedTopDUContext topContext);
            void setExpanded(bool);
        Q_SIGNALS:
          void navigateDeclaration(KDevelop::IndexedDeclaration);
        private slots:
            void labelClicked();
        private:
            IndexedTopDUContext m_topContext;
            IndexedDeclaration m_declaration;
            QPushButton* m_button;
    };

    /**
     * A widget that allows browsing through all the uses of a declaration
     */
    class KDEVPLATFORMLANGUAGE_EXPORT UsesWidget : public NavigatableWidgetList {
      Q_OBJECT
        public:
            UsesWidget(IndexedDeclaration declaration);
            ~UsesWidget();
        Q_SIGNALS:
            void navigateDeclaration(KDevelop::IndexedDeclaration);
        private:
            
            struct UsesWidgetCollector : public UsesCollector {
              public:
              UsesWidgetCollector(IndexedDeclaration decl, UsesWidget* widget);
              virtual void processUses(KDevelop::ReferencedTopDUContext topContext);
              virtual void maximumProgress(uint max);
              virtual void progress(uint processed, uint total);
              UsesWidget* m_widget;
            };
            UsesWidgetCollector* m_collector;
            QProgressBar* m_progressBar;
    };
}

#endif
