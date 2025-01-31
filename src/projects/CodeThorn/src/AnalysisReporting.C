#include "sage3basic.h"
#include "AnalysisReporting.h"
#include <iostream>
#include <string>
#include "CTAnalysis.h"
#include "AstStatistics.h"
#include "CppStdUtilities.h"
#include "ProgramLocationsAnalysis.h"
#include "BoolLattice.h"

using namespace std;
using namespace CodeThorn;

namespace CodeThorn {

  enum VerificationResult { INCONSISTENT, UNVERIFIED, VERIFIED, FALSIFIED };

  LabelSet AnalysisReporting::functionLabels(CodeThornOptions& ctOpt, CodeThorn::CTAnalysis* analyzer) {
    LabelSet allFunctionLabels;
    Flow& flow=*analyzer->getFlow();
    LabelSet functionEntryLabels=analyzer->getCFAnalyzer()->functionEntryLabels(flow);
    for(auto entryLabel : functionEntryLabels) {
      LabelSet funLabSet=analyzer->getCFAnalyzer()->functionLabelSet(entryLabel,flow);
      allFunctionLabels+=funLabSet;
    }
    return allFunctionLabels;
  }
  
  void AnalysisReporting::generateVerificationReports(CodeThornOptions& ctOpt, CodeThorn::CTAnalysis* analyzer, bool reportDetectedErrorLines) {
    for(auto analysisInfo : ctOpt.analysisList()) {
      AnalysisSelector analysisSel=analysisInfo.first;
      string analysisName=analysisInfo.second;
      if(ctOpt.getAnalysisSelectionFlag(analysisSel)) {
        cout<<endl;
        cout<<"-----------------------------------------------"<<endl;
        cout<<"Analysis results for "<<analysisName<<" analysis:"<<endl;
        cout<<"-----------------------------------------------"<<endl;
        ProgramLocationsReport report=analyzer->getExprAnalyzer()->getProgramLocationsReport(analysisSel);
#if 0
        LabelSet labelsOfInterest1=analyzer->getCFAnalyzer()->labelsOfInterestSet();
        // filter labels for dead code
        LabelSet labelsOfInterest2;
        for(auto lab : labelsOfInterest1) {
          SgNode* node=analyzer->getLabeler()->getNode(lab);
          if(SgLocatedNode* locNode=isSgLocatedNode(node))
            if(!SageInterface::insideSystemHeader(locNode))
              labelsOfInterest2.insert(lab);
        }
#else
        LabelSet labelsOfInterest2=AnalysisReporting::functionLabels(ctOpt, analyzer);
#endif
        report.setAllLocationsOfInterest(labelsOfInterest2);
        report.writeLocationsVerificationReport(cout,analyzer->getLabeler());
        cout<<"-----------------------------------------------"<<endl;
        // generate verification call graph
        AnalysisReporting::generateVerificationCallGraphDotFile(ctOpt,analyzer,analysisName,report);
        AnalysisReporting::generateVerificationFunctionsCsvFile(ctOpt,analyzer,analysisName,report);
        //report->writeFunctionsVerificationReport(cout,analyzer->getLabeler());
        cout<<"-----------------------------------------------"<<endl;
        if(reportDetectedErrorLines) {
          LabelSet violatingLabels=report.falsifiedLocations();
          if(report.numDefinitiveLocations()>0) {
            cout<<"Proven errors in program:"<<endl;
            report.writeAllDefinitiveLocationsToStream(cout,analyzer->getLabeler(),false,true,true);
          } else {
            LabelSet u=report.unverifiedLocations();
            if(u.size()==0) {
              cout<<"No violations exist - program verified."<<endl;
            } else {
              cout<<"No violations proven - violations may exist in unverified portion of program."<<endl;
            }
          }
        }
        cout<<"-----------------------------------------------"<<endl;
      }
    }
  }
  
  void AnalysisReporting::generateAnalysisStatsRawData(CodeThornOptions& ctOpt, CodeThorn::CTAnalysis* analyzer) {
    for(auto analysisInfo : ctOpt.analysisList()) {
      AnalysisSelector analysisSel=analysisInfo.first;
      string analysisName=analysisInfo.second;
      if(ctOpt.getAnalysisReportFileName(analysisSel).size()>0) {
        ProgramLocationsReport locations=analyzer->getExprAnalyzer()->getProgramLocationsReport(analysisSel);
        string fileName=ctOpt.getAnalysisReportFileName(analysisSel);
        if(!ctOpt.quiet)
          cout<<"Writing "<<analysisName<<" analysis results to file "<<fileName<<endl;
        locations.writeResultFile(fileName,analyzer->getLabeler());
      }
    }
  }
                             
  void AnalysisReporting::generateNullPointerAnalysisStats(CodeThorn::CTAnalysis* analyzer) {
    ProgramLocationsAnalysis pla;
    LabelSet pdlSet=pla.pointerDereferenceLocations(*analyzer->getLabeler());
    cout<<"Found "<<pdlSet.size()<<" pointer dereference locations."<<endl;
    
  }
  void AnalysisReporting::generateAstNodeStats(CodeThornOptions& ctOpt, SgProject* sageProject) {
    if(ctOpt.info.printAstNodeStats||ctOpt.info.astNodeStatsCSVFileName.size()>0) {
      // from: src/midend/astDiagnostics/AstStatistics.C
      if(ctOpt.info.printAstNodeStats) {
        ROSE_Statistics::AstNodeTraversalStatistics astStats;
        string s=astStats.toString(sageProject);
        cout<<s; // output includes newline at the end
      }
      if(ctOpt.info.astNodeStatsCSVFileName.size()>0) {
        ROSE_Statistics::AstNodeTraversalCSVStatistics astCSVStats;
        string fileName=ctOpt.info.astNodeStatsCSVFileName;
        astCSVStats.setMinCountToShow(1); // default value is 1
        if(!CppStdUtilities::writeFile(fileName, astCSVStats.toString(sageProject))) {
          cerr<<"Error: cannot write AST node statistics to CSV file "<<fileName<<endl;
          exit(1);
        }
      }
    }
  }

  void AnalysisReporting::generateAnalyzedFunctionsAndFilesReports(CodeThornOptions& ctOpt, CodeThorn::CTAnalysis* analyzer) {
    if(ctOpt.analyzedFunctionsCSVFileName.size()>0) {
      string fileName=ctOpt.analyzedFunctionsCSVFileName;
      if(!ctOpt.quiet)
        cout<<"Writing list of analyzed functions to file "<<fileName<<endl;
      string s=analyzer->analyzedFunctionsToString();
      if(!CppStdUtilities::writeFile(fileName, s)) {
        cerr<<"Cannot create file "<<fileName<<endl;
        exit(1);
      }
    }

    if(ctOpt.analyzedFilesCSVFileName.size()>0) {
      string fileName=ctOpt.analyzedFilesCSVFileName;
      if(!ctOpt.quiet)
        cout<<"Writing list of analyzed files to file "<<fileName<<endl;
      string s=analyzer->analyzedFilesToString();
      if(!CppStdUtilities::writeFile(fileName, s)) {
        cerr<<"Cannot create file "<<fileName<<endl;
        exit(1);
      }
    }

    if(ctOpt.externalFunctionsCSVFileName.size()>0) {
      string fileName=ctOpt.externalFunctionsCSVFileName;
      if(!ctOpt.quiet)
        cout<<"Writing list of external functions to file "<<fileName<<endl;
      string s=analyzer->externalFunctionsToString();
      if(!CppStdUtilities::writeFile(fileName, s)) {
        cerr<<"Cannot create file "<<fileName<<endl;
        exit(1);
      }
    }
  }
  void AnalysisReporting::generateVerificationCallGraphDotFile(CodeThornOptions& ctOpt, CodeThorn::CTAnalysis* analyzer, string analysisName, ProgramLocationsReport& report) {
    string fileName1=analysisName+"-cg1.dot";
    string fileName2=analysisName+"-cg2.dot";
    //cout<<"Generating verification call graph for "<<analysisName<<" analysis."<<endl;
    LabelSet verified=report.verifiedLocations();
    LabelSet falsified=report.falsifiedLocations();
    LabelSet unverified=report.unverifiedLocations();
    //cout<<"Verified  :"<<verified.toString()<<endl;
    //cout<<"Falsified :"<<falsified.toString()<<endl;
    //cout<<"Unverified:"<<unverified.toString()<<endl;
    Flow& flow=*analyzer->getFlow();
    LabelSet functionEntryLabels=analyzer->getCFAnalyzer()->functionEntryLabels(flow);
    std::map<Label,VerificationResult> fMap;
    for(auto entryLabel : functionEntryLabels) {
      // todo: function is
      // falsified: if at least one label is falsified (=0)
      // unverified: if at least one is unverified (=top)
      // verified: all labels are verified (=1)
      LabelSet funLabSet=analyzer->getCFAnalyzer()->functionLabelSet(entryLabel,flow);
      //cout<<"Function label set:"<<funLabSet.toString()<<endl;
      //for(auto lab : funLabSet) {
      //  cout<<lab.toString()<<":"<<analyzer->getLabeler()->getNode(lab)->class_name()<<endl;
      //}
      VerificationResult funVer=INCONSISTENT;
      size_t count=0;
      for(auto lab : funLabSet) {
        if(falsified.isElement(lab)) {
          funVer=FALSIFIED;
          break;
        }
        if(unverified.isElement(lab)) {
          funVer=UNVERIFIED;
        }
        if(verified.isElement(lab)) {
          count++;
        }
      }
      //cout<<"Function verification: "<<SgNodeHelper::getFunctionName(analyzer->getLabeler()->getNode(entryLabel))<<":"<<funLabSet.size()<<" vs "<<count<<endl;
      if(funLabSet.size()==count) {
        funVer=VERIFIED;
      }
      fMap[entryLabel]=funVer;
      //cout<<"DEBUG:entrylabel:"<<entryLabel.toString()<<" : "<<funVer<<" : "<< analyzer->getLabeler()->getNode(entryLabel)->unparseToString()<<endl;
    }
    InterFlow::LabelToFunctionMap map=analyzer->getCFAnalyzer()->labelToFunctionMap(flow);

    std::string cgBegin="digraph G {\n";
    std::string cgEnd="}\n";
    std::string cgEdges=analyzer->getInterFlow()->dotCallGraphEdges(map);
    // generate colored nodes
    std::string nodeColor;
    stringstream cgNodes;
    int numFalsifiedFunctions=0;
    int numUnverifiedFunctions=0;
    int numVerifiedFunctions=0;
    for (auto entryLabel : functionEntryLabels ) {
      switch(fMap[entryLabel]) {
      case FALSIFIED: nodeColor="red";numFalsifiedFunctions++;break;
      case UNVERIFIED: nodeColor="orange";numUnverifiedFunctions++;break;
      case VERIFIED: nodeColor="green";numVerifiedFunctions++;break;
      case INCONSISTENT: nodeColor="gray";break;
      }
      std::string functionName=SgNodeHelper::getFunctionName(analyzer->getLabeler()->getNode(entryLabel));
      std::string dotFunctionName="label=\""+entryLabel.toString()+":"+functionName+"\"";
      //if(nodeColor!="gray")
        cgNodes<<entryLabel.toString()<<" [style=filled, fillcolor="<<nodeColor<<","<<dotFunctionName<<"]"<<endl;
    }

    // without function name, only label id
    stringstream cgNodes2;
    for (auto entryLabel : functionEntryLabels ) {
      switch(fMap[entryLabel]) {
      case FALSIFIED: nodeColor="red";break;
      case UNVERIFIED: nodeColor="orange";break;
      case VERIFIED: nodeColor="green";break;
      case INCONSISTENT: nodeColor="gray";break;
      }
      std::string functionName=SgNodeHelper::getFunctionName(analyzer->getLabeler()->getNode(entryLabel));
      std::string dotFunctionName="label=\""+entryLabel.toString()+"\"";
      if(nodeColor!="gray")
        cgNodes2<<entryLabel.toString()<<" [style=filled, fillcolor="<<nodeColor<<","<<dotFunctionName<<"]"<<endl;
    }

    std::string dotFileString1=cgBegin+cgNodes2.str()+cgEdges+cgEnd;
    if(!CppStdUtilities::writeFile(fileName1, dotFileString1)) {
      cerr<<"Error: could not generate callgraph dot file "<<fileName1<<endl;
      exit(1);
    } else {
      cout<<"Generated verification call graph "<<fileName1<<endl;
    }

    std::string dotFileString2=cgBegin+cgNodes.str()+cgEdges+cgEnd;
    if(!CppStdUtilities::writeFile(fileName2, dotFileString2)) {
      cerr<<"Error: could not generate callgraph dot file "<<fileName2<<endl;
      exit(1);
    } else {
      cout<<"Generated verification call graph "<<fileName2<<endl;
    }

  }

  void AnalysisReporting::generateVerificationFunctionsCsvFile(CodeThornOptions& ctOpt, CodeThorn::CTAnalysis* analyzer, string analysisName, ProgramLocationsReport& report) {
    string fileName1=analysisName+"-functions.csv";
    LabelSet verified=report.verifiedLocations();
    LabelSet falsified=report.falsifiedLocations();
    LabelSet unverified=report.unverifiedLocations();
    
    Flow& flow=*analyzer->getFlow();
    enum VerificationResult { INCONSISTENT, UNVERIFIED, VERIFIED, FALSIFIED };
    std::map<Label,VerificationResult> fMap;
    LabelSet functionEntryLabels=analyzer->getCFAnalyzer()->functionEntryLabels(flow);
    for(auto entryLabel : functionEntryLabels) {
      LabelSet funLabSet=analyzer->getCFAnalyzer()->functionLabelSet(entryLabel,flow);
      VerificationResult funVer=INCONSISTENT;
      size_t count=0;
      for(auto lab : funLabSet) {
        if(falsified.isElement(lab)) {
          funVer=FALSIFIED;
          break;
        }
        if(unverified.isElement(lab)) {
          funVer=UNVERIFIED;
        }
        if(verified.isElement(lab)) {
          count++;
        }
      }
      if(funLabSet.size()==count) {
        funVer=VERIFIED;
      }
      fMap[entryLabel]=funVer;
    }
    InterFlow::LabelToFunctionMap map=analyzer->getCFAnalyzer()->labelToFunctionMap(flow);
    stringstream cgNodes;
    string nodeColor;
    int numFalsifiedFunctions=0;
    int numUnverifiedFunctions=0;
    int numVerifiedFunctions=0;
    int numInconsistentFunctions=0;
    for (auto entryLabel : functionEntryLabels ) {
      switch(fMap[entryLabel]) {
      case FALSIFIED: nodeColor="violated";numFalsifiedFunctions++;break;
      case UNVERIFIED: nodeColor="unverified";numUnverifiedFunctions++;break;
      case VERIFIED: nodeColor="verified";numVerifiedFunctions++;break;
      case INCONSISTENT: nodeColor="inconsistent";numInconsistentFunctions++;break;
      }
      SgNode* node=analyzer->getLabeler()->getNode(entryLabel);
      string fileName=SgNodeHelper::sourceFilenameToString(node);
      std::string functionName=SgNodeHelper::getFunctionName(node);
      //      if(nodeColor!="inconsistent")
        cgNodes<<fileName<<","<<functionName<<","<<nodeColor<<endl;
    }

    // print stats
    int numProvenFunctions=numVerifiedFunctions+numFalsifiedFunctions;
    int numTotalFunctions=numProvenFunctions+numUnverifiedFunctions+numInconsistentFunctions;
    cout<<"Proven       functions: "<<numProvenFunctions<<" [ "<<numProvenFunctions/(double)numTotalFunctions*100<<"%]"<<endl;
    cout<<" Verified    functions: "<<numVerifiedFunctions<<" [ "<<numVerifiedFunctions/(double)numTotalFunctions*100<<"%]"<<endl;
    cout<<" Violated    functions: "<<numFalsifiedFunctions<<" [ "<<numFalsifiedFunctions/(double)numTotalFunctions*100<<"%]"<<endl;
    cout<<"Unproven     functions: "<<numUnverifiedFunctions<<" [ "<<numUnverifiedFunctions/(double)numTotalFunctions*100<<"%]"<<endl;
    if(numInconsistentFunctions>0)
      cout<<"Inconsistent functions: "<<numInconsistentFunctions<<" [ "<<numInconsistentFunctions/(double)numTotalFunctions*100<<"%]"<<endl;
    cout<<"Total        functions: "<<numTotalFunctions<<endl;
    
    std::string dotFileString1=cgNodes.str();
    if(!CppStdUtilities::writeFile(ctOpt.csvReportModeString, fileName1, dotFileString1)) {
      cerr<<"Error: could not generate function verification file "<<fileName1<<endl;
      exit(1);
    } else {
      cout<<"Generated function verification file "<<fileName1<<endl;
    }

  }
  
} // end of namespace CodeThorn
