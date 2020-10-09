#include "edit-widget.h"


class EditOutputWidgetImpl : public EditOutputWidget
{
    QJsonObject conf_;
    
    QLineEdit* name_ = 0;
    QLineEdit* rtmp_path_ = 0;
    QLineEdit* rtmp_key_ = 0;

    QLineEdit* rtmp_user_ = 0;
    QLineEdit* rtmp_pass_ = 0;

    QComboBox* venc_ = 0;
    QLineEdit* v_bitrate_ = 0;
    QLineEdit* v_keyframe_sec_ = 0;
    QLineEdit* v_resolution_ = 0;
    QLabel* v_warning_ = 0;
    QComboBox* aenc_ = 0;
    QLineEdit* a_bitrate_ = 0;
    QComboBox* a_mixer_ = 0;

    std::vector<std::string> EnumEncodersByCodec(const char* codec)
    {
        if (!codec)
            return {};
        
        std::vector<std::string> result;
        int i = 0;
        for(;;)
        {
            const char* encid;
            if (!obs_enum_encoder_types(i++, &encid))
                break;
            auto enc_codec = obs_get_encoder_codec(encid);
            if (strcmp(enc_codec, codec) == 0)
                result.emplace_back(encid);
        }
        return result;
    }

public:
    EditOutputWidgetImpl(QJsonObject conf, QWidget* parent = 0)
        : QDialog(parent)
        , conf_(conf)
    {
        setWindowTitle(obs_module_text("StreamingSettings"));

        auto layout = new QGridLayout(this);
        layout->setColumnStretch(0, 0);
        layout->setColumnStretch(1, 1);

        int currow = 0;
        {
            layout->addWidget(new QLabel(obs_module_text("StreamingName"), this), currow, 0);
            layout->addWidget(name_ = new QLineEdit("", this), currow, 1);
        }
        ++currow;
        {
            layout->addWidget(new QLabel(obs_module_text("StreamingServer"), this), currow, 0);
            layout->addWidget(rtmp_path_ = new QLineEdit("", this), currow, 1);
        }
        ++currow;
        {
            layout->addWidget(new QLabel(obs_module_text("StreamingKey"), this), currow, 0);
            layout->addWidget(rtmp_key_ = new QLineEdit(u8"", this), currow, 1);
        }
        ++currow;
        {
            auto sub_layout = new QGridLayout(this);
            layout->addLayout(sub_layout, currow, 0, 1, 2);

            sub_layout->setColumnStretch(0, 0);
            sub_layout->setColumnStretch(1, 1);
            sub_layout->setColumnStretch(2, 0);
            sub_layout->setColumnStretch(3, 1);
            sub_layout->addWidget(new QLabel(obs_module_text("StreamingUser"), this), 0, 0);
            sub_layout->addWidget(rtmp_user_ = new QLineEdit(u8"", this), 0, 1);
            sub_layout->addWidget(new QLabel(obs_module_text("StreamingPassword"), this), 0, 2);
            sub_layout->addWidget(rtmp_pass_ = new QLineEdit(u8"", this), 0, 3);

            rtmp_user_->setPlaceholderText(obs_module_text("UsuallyNoNeed"));
            rtmp_pass_->setPlaceholderText(obs_module_text("UsuallyNoNeed"));
            rtmp_pass_->setEchoMode(QLineEdit::Password);
        }
        ++currow;
        {
            auto sub_grid = new QGridLayout(this);
            sub_grid->setColumnStretch(0, 1);
            sub_grid->setColumnStretch(1, 1);
            layout->addLayout(sub_grid, currow, 0, 1, 2);
            {
                {
                    auto gp = new QGroupBox(obs_module_text("VideoSettings"), this);
                    sub_grid->addWidget(gp, 0, 0);
                    auto encLayout = new QGridLayout(gp);
                    int currow = 0;
                    {
                        int curcol = 0;
                        encLayout->addWidget(new QLabel(obs_module_text("Encoder"), gp), currow, curcol++);
                        encLayout->addWidget(venc_ = new QComboBox(gp), currow, curcol++);
                    }
                    ++currow;
                    {
                        int curcol = 0;
                        encLayout->addWidget(new QLabel(obs_module_text("VideoResolution"), gp), currow, curcol++);
                        encLayout->addWidget(v_resolution_ = new QLineEdit("", gp), currow, curcol++);
                        v_resolution_->setPlaceholderText(obs_module_text("SameAsOBSNow"));
                    }
                    ++currow;
                    {
                        int curcol = 0;
                        encLayout->addWidget(new QLabel(obs_module_text("Bitrate"), gp), currow, curcol++);
                        auto c = new QGridLayout(gp);
                        {
                            int curcol = 0;
                            c->addWidget(v_bitrate_ = new QLineEdit("", gp), 0, curcol++);
                            c->addWidget(new QLabel("kbps", gp), 0, curcol++);
                            c->setColumnStretch(0, 1);
                            c->setColumnStretch(1, 0);
                        }
                        encLayout->addLayout(c, currow, curcol++);
                    }
                    ++currow;
                    {
                        int curcol = 0;
                        encLayout->addWidget(new QLabel(obs_module_text("KeyFrame"), gp), currow, curcol++);
                        auto c = new QGridLayout(gp);
                        {
                            int curcol = 0;
                            c->addWidget(v_keyframe_sec_ = new QLineEdit("", gp), 0, curcol++);
                            c->addWidget(new QLabel(obs_module_text("UnitSecond"), gp), 0, curcol++);
                            c->setColumnStretch(0, 1);
                            c->setColumnStretch(1, 0);
                        }
                        encLayout->addLayout(c, currow, curcol++);
                    }
                    ++currow;
                    {
                        encLayout->addWidget(v_warning_ = new QLabel(obs_module_text("Notice.CPUPower")), currow, 0, 1, 2);
                        v_warning_->setWordWrap(true);
                        v_warning_->setStyleSheet(u8"background-color: rgb(255,255,0); color: rgb(0,0,0)");
                    }
                    ++currow;
                    {
                        encLayout->addWidget(new QWidget(), currow, 0);
                        encLayout->setRowStretch(currow, 1);
                    }
                    gp->setLayout(encLayout);
                }

                {
                    auto gp = new QGroupBox(obs_module_text("AudioSettings"), this);
                    sub_grid->addWidget(gp, 0, 1);
                    auto encLayout = new QGridLayout(gp);
                    int currow = 0;
                    {
                        int curcol = 0;
                        encLayout->addWidget(new QLabel(obs_module_text("Encoder"), gp), currow, curcol++);
                        encLayout->addWidget(aenc_ = new QComboBox(gp), currow, curcol++);
                    }
                    ++currow;
                    {
                        int curcol = 0;
                        encLayout->addWidget(new QLabel(obs_module_text("Bitrate"), gp), currow, curcol++);
                        auto c = new QGridLayout(gp);
                        {
                            int curcol = 0;
                            c->addWidget(a_bitrate_ = new QLineEdit(gp), 0, curcol++);
                            c->addWidget(new QLabel(obs_module_text("kbps")), 0, curcol++);
                            c->setColumnStretch(0, 1);
                            c->setColumnStretch(1, 0);
                        }
                        encLayout->addLayout(c, currow, curcol++);
                    }
                    ++currow;
                    {
                        int curcol = 0;
                        encLayout->addWidget(new QLabel(obs_module_text("AudioMixerID"), gp), currow, curcol++);
                        encLayout->addWidget(a_mixer_ = new QComboBox(gp), currow, curcol++);

                        for(int i = 1; i <= 6; ++i)
                            a_mixer_->addItem(QString(std::to_string(i).c_str()), i - 1);
                    }
                    gp->setLayout(encLayout);
                }
            }
        }
        ++currow;
        {
            auto okbtn = new QPushButton(obs_module_text("OK"), this);
            QObject::connect(okbtn, &QPushButton::clicked, [this]() {
                SaveConfig();
                done(DialogCode::Accepted);
            });
            layout->addWidget(okbtn, currow, 0, 1, 2);
        }

        resize(540, 160);
        setLayout(layout);

        LoadEncoders();
        LoadConfig();
        ConnectWidgetSignals();
        UpdateUI();
    }

    QJsonObject Config() override
    {
        return conf_;
    }

    void ConnectWidgetSignals()
    {
        QObject::connect(venc_, (void (QComboBox::*)(int)) &QComboBox::currentIndexChanged, [this](){
            SaveConfig();
            LoadConfig();
            UpdateUI();
        });
        QObject::connect(aenc_, (void (QComboBox::*)(int)) &QComboBox::currentIndexChanged, [this](){
            SaveConfig();
            LoadConfig();
            UpdateUI();
        });
    }

    void LoadEncoders()
    {
        venc_->addItem(obs_module_text("SameAsOBS"), "");
        for(auto x : EnumEncodersByCodec("h264"))
            venc_->addItem(obs_encoder_get_display_name(x.c_str()), x.c_str());
        
        aenc_->addItem(obs_module_text("SameAsOBS"), "");
        for(auto x : EnumEncodersByCodec("AAC"))
            aenc_->addItem(obs_encoder_get_display_name(x.c_str()), x.c_str());
    }

    void UpdateUI()
    {
        auto ve = venc_->currentData();
        if (ve.isValid() && ve.toString() == "")
        {
            v_bitrate_->setText(obs_module_text("SameAsOBS"));
            v_bitrate_->setEnabled(false);
            v_resolution_->setText(obs_module_text("SameAsOBS"));
            v_resolution_->setEnabled(false);
            v_keyframe_sec_->setEnabled(false);
            v_keyframe_sec_->setText(obs_module_text("SameAsOBS"));
            v_warning_->setVisible(false);
        }
        else
        {
            v_bitrate_->setEnabled(true);
            v_resolution_->setEnabled(true);
            v_keyframe_sec_->setEnabled(true);
            v_warning_->setVisible(true);
        }

        auto ae = aenc_->currentData();
        if (ae.isValid() && ae.toString() == "")
        {
            a_bitrate_->setText(obs_module_text("SameAsOBS"));
            a_bitrate_->setEnabled(false);
            a_mixer_->setEnabled(false);
        }
        else
        {
            a_bitrate_->setEnabled(true);
            a_mixer_->setEnabled(true);
        }
    }

    void SaveConfig()
    {
        conf_["name"] = name_->text();
        conf_["rtmp-path"] = rtmp_path_->text();
        conf_["rtmp-key"] = rtmp_key_->text();
        conf_["rtmp-user"] = rtmp_user_->text();
        conf_["rtmp-pass"] = rtmp_pass_->text();
        conf_["v-enc"] = venc_->currentData().toString();
        conf_["a-enc"] = aenc_->currentData().toString();
        if (v_bitrate_->isEnabled())
            try { conf_["v-bitrate"] = std::stod(tostdu8(v_bitrate_->text())); } catch(...) {}
        if (v_keyframe_sec_->isEnabled())
            try { conf_["v-keyframe-sec"] = std::stod(tostdu8(v_keyframe_sec_->text())); } catch(...) {}
        if (v_resolution_->isEnabled())
            conf_["v-resolution"] = v_resolution_->text();
        if (a_bitrate_->isEnabled())
            try { conf_["a-bitrate"] = std::stod(tostdu8(a_bitrate_->text())); } catch(...) {}
        if (a_mixer_->isEnabled())
            conf_["a-mixer"] = a_mixer_->currentData().toDouble();
    }

    void LoadConfig()
    {
        auto it = conf_.find("name");
        if (it != conf_.end() && it->isString())
            name_->setText(it->toString());
        else
            name_->setText(obs_module_text("NewStreaming"));
        
        it = conf_.find("rtmp-path");
        if (it != conf_.end() && it->isString())
            rtmp_path_->setText(it->toString());
        
        it = conf_.find("rtmp-key");
        if (it != conf_.end() && it->isString())
            rtmp_key_->setText(it->toString());

        it = conf_.find("rtmp-user");
        if (it != conf_.end() && it->isString())
            rtmp_user_->setText(it->toString());

        it = conf_.find("rtmp-pass");
        if (it != conf_.end() && it->isString())
            rtmp_pass_->setText(it->toString());

        it = conf_.find("v-enc");
        if (it != conf_.end() && it->isString())
        {
            int idx = venc_->findData(it->toString());
            if (idx >= 0)
                venc_->setCurrentIndex(idx);
        }

        it = conf_.find("v-bitrate");
        if (it != conf_.end() && it->isDouble())
            v_bitrate_->setText(std::to_string((int)it->toDouble()).c_str());
        else
            v_bitrate_->setText("2000");

        it = conf_.find("v-keyframe-sec");
        if (it != conf_.end() && it->isDouble())
            v_keyframe_sec_->setText(std::to_string((int)it->toDouble()).c_str());
        else
            v_keyframe_sec_->setText("3");
        
        it = conf_.find("v-resolution");
        if (it != conf_.end() && it->isString())
            v_resolution_->setText(it->toString());
        else
            v_resolution_->setText("");
        
        it = conf_.find("a-enc");
        if (it != conf_.end() && it->isString())
        {
            int idx = aenc_->findData(it->toString());
            if (idx >= 0)
                aenc_->setCurrentIndex(idx);
        }

        it = conf_.find("a-bitrate");
        if (it != conf_.end() && it->isDouble())
            a_bitrate_->setText(std::to_string((int)it->toDouble()).c_str());
        else
            a_bitrate_->setText("128");

        it = conf_.find("a-mixer");
        {
            int dataToFind = 1;
            if (it != conf_.end() && it->isDouble())
                dataToFind = (int)it->toDouble();
            int index = a_mixer_->findData(dataToFind);
            if (index >= 0)
                a_mixer_->setCurrentIndex(index);
            else
                a_mixer_->setCurrentIndex(0);
        }
    }
};

EditOutputWidget* createEditOutputWidget(QJsonObject conf, QWidget* parent) {
    return new EditOutputWidgetImpl(conf, parent);
}
