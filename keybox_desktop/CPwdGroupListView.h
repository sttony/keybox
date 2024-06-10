

#ifndef KEYBOX_CPWDGROUPLISTVIEW_H
#define KEYBOX_CPWDGROUPLISTVIEW_H


#include <QListView>

class CPwdGroupListView : public QListView{
public:
    explicit CPwdGroupListView(QWidget *parent = nullptr) : QListView(parent) {}
};


#endif //KEYBOX_CPWDGROUPLISTVIEW_H
