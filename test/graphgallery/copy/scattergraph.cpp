#include <QtWidgets/qboxlayout.h>
#include <QtWidgets/qcheckbox.h>
#include <QtWidgets/qcombobox.h>
#include <QtWidgets/qlabel.h>
#include <QtWidgets/qcommandlinkbutton.h>

#include "scattergraph.h"
#include "scatterdatamodifier.h"

using namespace Qt::StringLiterals;

ScatterGraph::ScatterGraph(){ m_scatterGraph = new Q3DScatter(); }

ScatterGraph::~ScatterGraph() = default;

bool ScatterGraph::initialize(QSize const& minimum_grp_sz
    ,QSize const& maximum_grp_sz){
  if (!m_scatterGraph->hasContext()) return false;
  m_scatterWidget = new QWidget;
  auto* hlayout = new QHBoxLayout(m_scatterWidget);
  m_container->setMinimumSize(minimum_grp_sz);
  m_container->setMaximumSize(maximum_grp_sz);
  m_container->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
  m_container->setFocusPolicy(Qt::StrongFocus);
  hlayout->addWidget(m_container,1);

  auto* vlayout = new QVBoxLayout();
  hlayout->addLayout(vlayout);

  auto* camera_btn = new QCommandLinkButton(m_scatterWidget);
  camera_btn->setText("change camera preset");
  camera_btn->setDescription(u"camera_btn->setDescription"_s);
  camera_btn->setIconSize(QSize(0,0));

  auto* item_ctn_btn = new QCommandLinkButton(m_scatterWidget);
  item_ctn_btn->setText(u"Toggle item count"_s);
  item_ctn_btn->setDescription(u"Switch between 900 and 10000 data points"_s);
  item_ctn_btn->setIconSize(QSize(0, 0));

  auto* range_btn = new QCommandLinkButton(m_scatterWidget);
  range_btn->setText(u"toggle axis ranges"_s);
  range_btn->setDescription(u"Switch between automatic axis ranges and preset ranges"_s);
  range_btn->setIconSize(QSize(0, 0));

  auto* bk_ckb = new QCheckBox(m_scatterWidget);
  bk_ckb->setText(u"show background"_s);
  bk_ckb->setChecked(true);

  auto* grid_ckb = new QCheckBox(m_scatterWidget);
  grid_ckb->setText(u"show grid"_s);
  grid_ckb->setChecked(true);

  auto* smooth_ckb = new QCheckBox(m_scatterWidget);
  smooth_ckb->setText(u"smooth dots"_s);
  smooth_ckb->setChecked(true);

  auto* item_style_list = new QComboBox(m_scatterWidget);
  item_style_list->addItem(u"Sphere"_s, QAbstract3DSeries::MeshSphere);
  item_style_list->addItem(u"Cube"_s, QAbstract3DSeries::MeshCube);
  item_style_list->addItem(u"Minimal"_s, QAbstract3DSeries::MeshMinimal);
  item_style_list->addItem(u"Point"_s, QAbstract3DSeries::MeshPoint);

  auto* theme_list = new QComboBox(m_scatterWidget);
  theme_list->addItem(u"Qt"_s);
  theme_list->addItem(u"Primary Colors"_s);
  theme_list->addItem(u"Digia"_s);
  theme_list->addItem(u"Stone Moss"_s);
  theme_list->addItem(u"Army Blue"_s);
  theme_list->addItem(u"Retro"_s);
  theme_list->addItem(u"Ebony"_s);
  theme_list->addItem(u"Isabelle"_s);
  theme_list->setCurrentIndex(3);
  
  auto* shadow_quality = new QComboBox(m_scatterWidget);
  shadow_quality->addItem(u"None"_s);
  shadow_quality->addItem(u"Low"_s);
  shadow_quality->addItem(u"Medium"_s);
  shadow_quality->addItem(u"High"_s);
  shadow_quality->addItem(u"Low Soft"_s);
  shadow_quality->addItem(u"Medium Soft"_s);
  shadow_quality->addItem(u"High Soft"_s);
  shadow_quality->setCurrentIndex(6);

  vlayout->addWidget(camera_btn);
  vlayout->addWidget(item_ctn_btn);
  vlayout->addWidget(range_btn);
  vlayout->addWidget(bk_ckb);
  vlayout->addWidget(grid_ckb);
  vlayout->addWidget(smooth_ckb);
  vlayout->addWidget(new QLabel(u"change dot style"_s));
  vlayout->addWidget(item_style_list);
  vlayout->addWidget(new QLabel(u"change theme"_s));
  vlayout->addWidget(theme_list);
  vlayout->addWidget(new QLabel(u"adjust shadow quality"_s));
  vlayout->addWidget(shadow_quality,1,Qt::AlignTop);

  auto* modifier = new ScatterDataModifier(m_scatterGraph,this);
  QObject::connect(camera_btn,&QCommandLinkButton::clicked
      ,modifier,&ScatterDataModifier::changePresetCamera);
  QObject::connect(item_ctn_btn,&QCommandLinkButton::clicked
      ,modifier,&ScatterDataModifier::toggleItemCount);
  QObject::connect(range_btn,&QCommandLinkButton::clicked
      ,modifier,&ScatterDataModifier::toggleRanges);
  QObject::connect(bk_ckb,&QCheckBox::stateChanged
      ,modifier,&ScatterDataModifier::setBackgroundEnabled);
  QObject::connect(grid_ckb,&QCheckBox::stateChanged
      ,modifier,&ScatterDataModifier::setGridEnabled);
  QObject::connect(smooth_ckb,&QCheckBox::stateChanged
      ,modifier,&ScatterDataModifier::setSmoothDots);
  QObject::connect(modifier,&ScatterDataModifier::backgroundEnabledChanged
      ,bk_ckb,&QCheckBox::setChecked);
  QObject::connect(modifier,&ScatterDataModifier::gridEnabledChanged
      ,grid_ckb,&QCheckBox::setChecked);
  QObject::connect(item_style_list,&QComboBox::currentIndexChanged
      ,modifier,&ScatterDataModifier::changeStyle);
  QObject::connect(theme_list,&QComboBox::currentIndexChanged
      ,modifier,&ScatterDataModifier::changeTheme);
  QObject::connect(shadow_quality,&QComboBox::currentIndexChanged
      ,modifier,&ScatterDataModifier::changeShadowQuality);
  QObject::connect(modifier,&ScatterDataModifier::shadowQualityChanged
      ,shadow_quality,&QComboBox::setCurrentIndex);
  QObject::connect(m_scatterGraph,&Q3DScatter::shadowQualityChanged
      ,modifier,&ScatterDataModifier::shadowQualityUpdatedByVisual);
  return true;
}
