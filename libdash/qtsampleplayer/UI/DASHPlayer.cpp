/*
 * DASHPlayer.cpp
 *****************************************************************************
 * Copyright (C) 2012, bitmovin Softwareentwicklung OG, All Rights Reserved
 *
 * Email: libdash-dev@vicky.bitmovin.net
 *
 * This source code and its use and distribution, is subject to the terms
 * and conditions of the applicable license agreement.
 *****************************************************************************/

#include "DASHPlayer.h"
#include <iostream>

using namespace libdash::framework::adaptation;
using namespace sampleplayer;
using namespace sampleplayer::renderer;
using namespace sampleplayer::managers;
using namespace dash::mpd;
using namespace std;

DASHPlayer::DASHPlayer  (QtSamplePlayerGui& gui) : 
                        gui(&gui)
{
    this->videoElement      = gui.GetVideoElement();
    this->multimediaManager = new MultimediaManager(this->videoElement);

    this->multimediaManager->AttachVideoBufferObserver(this);
    this->gui->AddWidgetObserver(this);
    this->OnURLEntered(NULL, this->gui->GetUrl());

    QObject::connect(this, SIGNAL(FillStateChanged(int)), &gui, SLOT(SetBufferFillState(int)));
}
DASHPlayer::~DASHPlayer ()
{
    this->multimediaManager->Stop();
    delete(this->multimediaManager);
}

void DASHPlayer::OnStartButtonPressed   (QtSamplePlayerGui* widget)
{
    this->multimediaManager->Start();
}
void DASHPlayer::OnStopButtonPressed    (QtSamplePlayerGui* widget)
{
    this->multimediaManager->Stop();
}
void DASHPlayer::OnCheckboxChanged      (QtSamplePlayerGui* widget, bool isAutomatic)
{

}
void DASHPlayer::OnSettingsChanged      (QtSamplePlayerGui* widget, int video_adaption, int video_representation)
{
    if(this->multimediaManager->GetMPD() == NULL)
        return; // TODO dialog or symbol that indicates that error

    IPeriod *currentPeriod = this->multimediaManager->GetMPD()->GetPeriods().at(0);

    this->multimediaManager->SetVideoAdaptationSet(currentPeriod->GetAdaptationSets().at(video_adaption));
    this->multimediaManager->SetVideoRepresenation(currentPeriod->GetAdaptationSets().at(video_adaption)->GetRepresentation().at(video_representation));
}
void DASHPlayer::OnURLEntered           (QtSamplePlayerGui* widget, const std::string& url)
{
    if(!this->multimediaManager->Init(url))
    {
        this->gui->SetStatusBar("Error parsing mpd at: " + url);
        return; // TODO dialog or symbol that indicates that error
    }

    this->gui->SetStatusBar("Successfully parsed MPD at: " + url);
    this->gui->SetGuiFields(this->multimediaManager->GetMPD());

    this->InitMultimediaStreams();
}
void DASHPlayer::InitMultimediaStreams  ()
{
    if(this->multimediaManager->GetMPD() == NULL)
        return; // TODO dialog or symbol that indicates that error

    IPeriod *currentPeriod = this->multimediaManager->GetMPD()->GetPeriods().at(0);
    this->multimediaManager->SetVideoAdaptationSet(currentPeriod->GetAdaptationSets().at(0));
    this->multimediaManager->SetAudioRepresenation(currentPeriod->GetAdaptationSets().at(0)->GetRepresentation().at(0));
}
void DASHPlayer::OnBufferStateChanged   (uint32_t fillstateInPercent)
{
    emit FillStateChanged(fillstateInPercent);
}