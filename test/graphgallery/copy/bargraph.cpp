#include <QtWidgets/qboxlayout.h>
#include <QtWidgets/qpushbutton.h>
#include <QtWidgets/qslider.h>
#include <QtWidgets/qslider.h>
#include <QtWidgets/qlabel.h>
#include <QtWidgets/qradiobutton.h>
#include <QtWidgets/qbuttongroup.h>
#include <QtGui/qfontdatabase.h>

#include "bargraph.h"
#include "graphmodifier.h"

using namespace Qt::StringLiterals;
BarGraph::BarGraph(){ m_barsGraph = new Q3DBars(); }
BarGraph::~BarGraph()=default;

bool BarGraph::initialize(QSize const& minimum_gs, QSize const& maximum_gs){
  if (!m_barsGraph->hasContext()) return false;
  m_barsWidget = new QWidget;
  auto* hlayout = new QHBoxLayout(m_barsWidget);
  m_container = QWidget::createWindowContainer(m_barsGraph,m_barsWidget);
  m_barsGraph->resize(minimum_gs);
  m_container->setMinimumSize(minimum_gs);
  m_container->setMaximumSize(maximum_gs);
  m_container->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
  hlayout->addWidget(m_container,1);

  auto* vlayout = new QVBoxLayout();
  hlayout->addLayout(vlayout);

  auto* themes = new QComboBox(m_barsWidget);
  themeList->addItem(u"Qt"_s);
  themeList->addItem(u"Primary Colors"_s);
  themeList->addItem(u"Digia"_s);
  themeList->addItem(u"Stone Moss"_s);
  themeList->addItem(u"Army Blue"_s);
  themeList->addItem(u"Retro"_s);
  themeList->addItem(u"Ebony"_s);
  themes->addItem(u"Isabelle"_s);
  themes->setCurrentIndex(0);

  auto* label_bt = new QPushButton(m_barsWidget);*
  label_bt->setText(u"Change label style"_s);

  auto* smooth_cb = new QCheckBox(m_barsWidget);
  smooth_cb->setText(u"Smooth bars"_s);
  smooth_cb->setChecked(false);

  auto* bar_style_cbb = new QComboBox(m_barsWidget);
  bar_style_cbb->addItem(u"Bar"_s, QAbstract3DSeries::MeshBar);
  bar_style_cbb->addItem(u"Pyramid"_s, QAbstract3DSeries::MeshPyramid);
  bar_style_cbb->addItem(u"Cone"_s, QAbstract3DSeries::MeshCone);
  bar_style_cbb->addItem(u"Cylinder"_s, QAbstract3DSeries::MeshCylinder);
  bar_style_cbb->addItem(u"Bevel bar"_s, QAbstract3DSeries::MeshBevelBar);
  bar_style_cbb->addItem(u"Sphere"_s, QAbstract3DSeries::MeshSphere);
  bar_style_cbb->setCurrentIndex(4);

  auto* camera_btn = new QPushButton(m_barsWidget);
  camera_btn->setText(u"..."_s);
  auto* zoom_to_seleted_btn = new QPushButton(m_barsWidget);
  zoom_to_seleted_btn->setText(u"..."_s);

  auto* sm_ls = new QComboBox(m_barsWidget);
  sm_ls->addItem(u"None"_s
      ,int(QAbstract3DSeries:SelectionNone));
  sm_ls->addItem(u"Bar"_s
      ,int(QAbstract3DSeries:SelectionItem));
  //...
  sm_ls->setCurrentIndex(1);

  auto* bk_ckb = new QCheckBox(m_barsWidget);
  bk_ckb->setText(u"show background"_s);
  bk_ckb->setChecked(false);
  auto* gd_ckb = new QCheckBox(m_barsWidget);
  gd_ckb->setText(u"show grid"_s);
  gd_ckb->setChecked(true);
  auto* series_ckb = new QComboBox(m_barsWidget);
  series_ckb->setText(u"show second series"_s);
  series_ckb->setChecked(false);
  auto* reverse_axis_ckb = new QCheckBox(Qt::Horizontal,m_barsWidget);
  reverse_axis_ckb->setText(u"reverse value axis"_s);
  reverse_axis_ckb->setChecked(false);
  auto* reflection_ckb = new QCheckBox(m_barsWidget);
  reflection_ckb->setText(u"show reflections"_s);
  reflection_ckb->setChecked(false);

  auto* rotation_slider_x = new QCheckBox(m_barsWidget);
  rotation_slider_x->setTickInterval(30);
  rotation_slider_x->setTickPosition(QSlider::TicksBelow);
  rotation_slider_x->setMinimum(-180);
  rotation_slider_x->setValue(0);
  rotation_slider_x->setMaximum(180);
  auto* rotation_slider_y = new QCheckBox(m_barsWidget);
  rotation_slider_x->setTickInterval(15);
  rotation_slider_x->setTickPosition(QSlider::TicksAbove);
  rotation_slider_x->setMinimum(-90);
  rotation_slider_x->setValue(0);
  rotation_slider_x->setMaximum(90);
  auto* font_sz_slider=new QSlider(Qt::Horizontal,m_barsWidget);
  font_sz_slider->setTickInterval(10);
  font_sz_slider->setTickPosition(QSlider::TicksBelow);
  font_sz_slider->setMinimum(1);
  font_sz_slider->setMaximum(100);
  font_sz_slider->setValue(30);

  auto* font_ls  = new QFontComboBox(m_barsWidget);
  font_ls->setCurrentFont(QFont("times new roman"));

  auto* shadow_quality = new QComboBox(m_barsWidget);
  shadow_quality->addItem(u"None"_s);
  shadow_quality->addItem(u"Low"_s);
  shadow_quality->addItem(u"Medium"_s);
  shadow_quality->addItem(u"High"_s);
  shadow_quality->addItem(u"Low Soft"_s);
  shadow_quality->addItem(u"Medium Soft"_s);
  shadow_quality->addItem(u"High Soft"_s);
  shadow_quality->setCurrentIndex(5);

  auto* range_cbb = new QComboBox(m_barsWidget);
  range_cbb->addItem(u"2015"_s);
  range_cbb->addItem(u"2016"_s);
  range_cbb->addItem(u"2017"_s);
  range_cbb->addItem(u"2018"_s);
  range_cbb->addItem(u"2019"_s);
  range_cbb->addItem(u"2020"_s);
  range_cbb->addItem(u"2021"_s);
  range_cbb->addItem(u"2022"_s);
  range_cbb->addItem(u"All"_s);
  range_cbb->setCurrentIndex(8);

  auto* axis_title_visible_ckb = new QCheckBox(m_barsWidget);
  axis_title_visible_ckb->setText(u"axis titles visible"_s);
  axis_title_visible_ckb->setChecked(true);
  auto* axis_title_fixed_ckb = new QCheckBox(m_barsWidget);
  axis_title_fixed_ckb->setText(u"axis titles fixed"_s);
  axis_title_fixed_ckb->setChecked(true);
  auto* axis_label_rotation_slider = new QSlider(Qt::Horizontal,m_barsWidget);
  axis_label_rotation_slider->setTickInterval(10);
  axis_label_rotation_slider->setTickPosition(QSlider::TicksBelow);
  axis_label_rotation_slider->setMinimum(0);
  axis_label_rotation_slider->setMaximum(90);
  axis_label_rotation_slider->setValue(30);

  auto* mode_group_btg = new QButtonGroup(m_barsWidget);
  auto* mode_weather = new QRadioButton(u"temperature data"_s,m_barsWidget);
  mode_weather->setChecked(true);
  auto* mode_custom_proxy = new QRadioButton(u"custom proxy data"_s,m_barsWidget);
  mode_group_btg->addButton(mode_weather);
  mode_group_btg->addButton(mode_custom_proxy);

  vlayout->addWidget(new QLabel(u"roate horizontally"_s));
  vlayout->addWidget(rotation_slider_x,0,Qt::AlignTop);
  vlayout->addWidget(label_bt,0,Qt::AlignTop);
  vlayout->addWidget(camera_btn,0,Qt::AlignTop);
  vlayout->addWidget(zoom_to_seleted_btn,0,Qt::AlignTop);
  vlayout->addWidget(bk_ckb);
  vlayout->addWidget(gd_ckb);
  vlayout->addWidget(smooth_cb);
  vlayout->addWidget(reflection_ckb);
  vlayout->addWidget(series_ckb);
  vlayout->addWidget(reverse_axis_ckb);
  vlayout->addWidget(axis_title_visible_ckb);
  vlayout->addWidget(axis_title_fixed_ckb);

  vlayout->addWidget(new QLabel(u"show year"_s));
  vlayout->addWidget(range_cbb);
  vlayout->addWidget(new QLabel(u"change bar "_s));
  vlayout->addWidget(bar_style_cbb);
  vlayout->addWidget(new QLabel(u"change selection mode"_s));
  vlayout->addWidget(sm_ls);
  vlayout->addWidget(new QLabel(u"change theme"_s));
  vlayout->addWidget(themes);
  vlayout->addWidget(new QLabel(u"adjust shadow quality"_s));
  vlayout->addWidget(shadow_quality);
  vlayout->addWidget(new QLabel(u"change font"_s));
  vlayout->addWidget(font_ls);
  vlayout->addWidget(new QLabel(u"adjust font size"_s));
  vlayout->addWidget(font_sz_slider);
  vlayout->addWidget(new QLabel(u"axis label rotation"_s));
  vlayout->addWidget(axis_label_rotation_slider,0,Qt::AlignTop);
  vlayout->addWidget(mode_weather,0,Qt::AlignTop);
  vlayout->addWidget(mode_custom_proxy,0,Qt::AlignTop);

  auto* modifier = new GraphModifier(m_barsGraph,this);
  QObject::connect(rotation_slider_x,QSlider::valueChanged
      ,modifier,&GraphModifier::rotateX);
  QObject::connect(rotation_slider_y,QSlider::valueChanged
      ,modifier,&GraphModifier::rotateY);
  QObject::connect(label_bt,&QPushButton::clicked
      ,modifier,&GraphModifier::changeLabelBackground);
  QObject::connect(camera_btn,&QPushButton::clicked
      ,modifier,&GraphModifier::changePresetCamera);
  QObject::connect(zoom_to_seleted_btn,&QPushButton::clicked
      ,modifier,&GraphModifier::zoomToSelectedBar);

  QObject::connect(bk_ckb,&QCheckBox::stateChanged
      ,modifier,&GraphModifier::setBackgroundEnabled);
  QObject::connect(gd_ckb,&QCheckBox::stateChanged
      ,modifier,&GraphModifier::setGridEnabled);
  QObject::connect(smooth_cb,&QCheckBox::stateChanged
      ,modifier,&GraphModifier::setSmoothBars);
  QObject::connect(series_ckb,&QCheckBox::stateChanged
      ,modifier,&GraphModifier:;setSeriesVisibility);
  QObject::connect(reverse_axis_ckb,&QCheckBox::stateChanged
      modifier,&GraphModifier::setReverseValueAxis);
  QObject::connect(reflection_ckb,&QCheckBox::stateChanged
      ,modifier,&GraphModifier::setReflection);
  QObject::connect(modifier,&GraphModifier::backgroundEnabledChanged
      ,bk_ckb,&QCheckBox::setChecked);
  QObject::connect(modifier,&GraphModifier::gridEnabledChanged
      ,gd_ckb,&QCheckBox::setChecked);
  QObject::connect(range_cbb,&QComboBox::currentIndexChanged
      ,modifier,&GraphModifier::changeRange);
  QObject::connect(bar_style_cbb,&QComboBox::currentIndexChanged
      ,modifier,&GraphModifier::changeStyle);
  QObject::connect(sm_ls,*QComboBox::currentIndexChanged
      ,modifier,&GraphModifier::changeSelectionMode);
  QObject::connect(themes,&QComboBox::currentIndexChanged
      ,modifier,&GraphModifier::changeTheme);
  QObject::connect(shadow_quality,&QComboBox::currentIndexChanged
      ,modifier,&GraphModifier::changeShadowQuality);
  QObject::connect(modifier,&GraphModifier::shadowQualityChanged
      ,shadow_quality,&QComboBox::setCurrentIndex);
  QObject::connect(m_barsGraph,&Q3DBars::shadowQualityChanged
      ,modifier,&GraphModifier::shadowQualityUpdatedByVisual);
  QObject::connect(font_sz_slider,&QSlider::valueChanged
      ,modifier,&GraphModifier::changeFontSize);
  QObject::connect(font_ls,&QFontComboBox::currentFontChanged
      ,modifier,&GraphModifier::changeFont);
  QObject::connect(modifier,&GraphModifier::fontSizeChanged
      ,font_sz_slider,&QSlider::setValue);
  QObject::connect(modifier,&GraphModifier::fontChanged
      ,font_ls,&QFontComboBox::setCurrentFont);
  QObject::connect(axis_title_visible_ckb,&QCheckBox::stateChanged
      ,modifier,&GraphModifier::setAxisTitleVisibility);
  QObject::connect(axis_title_fixed_ckb,&QCheckBox::stateChanged
      ,modifier,&GraphModifier::setAxisTitleFixed);
  QObject::connect(axis_label_rotation_slider,&QSlider::valueChanged
      ,modifier,&GraphModifier::setDataModeToCustom);
  QObject::connect(mode_custom_proxy,&QRadioButton::toggled
      ,modifier,&GraphModifier::setDataModeToWeather);

  QObject::connect(mode_weather,&QRadioButton::toggled
      ,modifier,&GraphModifier::setDataModeToCustom);
  QObject::connect(mode_custom_proxy,&QRadioButton::toggled
      ,modifier,&GraphModifier::setDataModeToCustom);
  QObject::connect(mode_weather,&QRadioButton::toggled
      ,series_ckb,&QCheckBox::setEnabled);
  QObject::connect(mode_weather,&QRadioButton::toggled,range_cbb,QComboBox::setEnabled);
  QObject::connect(mode_weather,&QRadioButton::toggled,axis_title_visible_ckb
      ,&QComboBox::setEnabled);
  QObject::connect(mode_weather,&QRadioButton::toggled,axis_title_fixed_ckb
      ,&QComboBox::setEnabled);
  QObject::connect(mode_weather,&QRadioButton::toggled,axis_label_rotation_slider
      ,&QSlider::setEnabled);
  return true;
}
