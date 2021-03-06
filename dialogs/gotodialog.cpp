#include "gotodialog.h"
#include "ui_gotodialog.h"
#include "../models/gotomodel.h"

GotoDialog::GotoDialog(REDasm::DisassemblerAPI *disassembler, QWidget *parent) : QDialog(parent), ui(new Ui::GotoDialog), m_disassembler(disassembler), m_address(0), m_validaddress(false)
{
    ui->setupUi(this);

    m_gotomodel = ListingFilterModel::createFilter<GotoModel>(ui->tvFunctions);
    m_gotomodel->setDisassembler(disassembler);

    ui->tvFunctions->setModel(m_gotomodel);
    ui->tvFunctions->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    ui->tvFunctions->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);

    connect(ui->leAddress, &QLineEdit::textChanged, this, [=](const QString) { this->validateEntry(); });
    connect(ui->leAddress, &QLineEdit::returnPressed, this, &GotoDialog::accept);

    connect(ui->tvFunctions, &QTableView::doubleClicked, this, &GotoDialog::onSymbolSelected);
    connect(ui->tvFunctions, &QTableView::doubleClicked, this, &GotoDialog::accept);

    connect(ui->pbGoto, &QPushButton::clicked, this, &GotoDialog::accept);
}

GotoDialog::~GotoDialog() { delete ui; }
address_t GotoDialog::address() const { return m_address; }
bool GotoDialog::hasValidAddress() const { return m_validaddress && m_disassembler->document()->segment(m_address); }

void GotoDialog::validateEntry()
{
    bool ok = false;
    QString s = ui->leAddress->text();

    if(s.isEmpty())
    {
        m_validaddress = false;
        ui->pbGoto->setEnabled(false);
        m_gotomodel->clearFilter();
        return;
    }

    m_address = s.toULongLong(&ok, 16);
    ui->pbGoto->setEnabled(ok);
    m_validaddress = ok;
    m_gotomodel->setFilter(s);
}

void GotoDialog::onSymbolSelected(const QModelIndex &index)
{
    m_validaddress = false;
    emit symbolSelected(index);
}
