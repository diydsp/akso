package axoloti.piccolo.patch.object.display;

import axoloti.abstractui.IAxoObjectInstanceView;
import axoloti.patch.object.display.DisplayInstance;
import axoloti.patch.object.display.DisplayInstance1;
import axoloti.piccolo.components.PLabelComponent;
import java.awt.Dimension;
import java.beans.PropertyChangeEvent;
import axoloti.datatypes.Value;
import axoloti.datatypes.ValueFrac32;
import axoloti.patch.object.display.DisplayInstanceNoteLabel;

class PDisplayInstanceViewNoteLabel extends PDisplayInstanceViewFrac32 {

    private DisplayInstanceNoteLabel displayInstance;

    PDisplayInstanceViewNoteLabel(DisplayInstance displayInstance, IAxoObjectInstanceView axoObjectInstanceView) {
        super(displayInstance, axoObjectInstanceView);
        initComponents();
        this.displayInstance = (DisplayInstanceNoteLabel) displayInstance;
    }

    private PLabelComponent readout;

    private void initComponents() {
        readout = new PLabelComponent("xxxxx");
        addChild(readout);
        readout.setSize(new Dimension(40, 18));
    }

    @Override
    public void modelPropertyChange(PropertyChangeEvent evt) {
        super.modelPropertyChange(evt);
        if (DisplayInstance.DISP_VALUE.is(evt)) {
            //throw new UnsupportedOperationException("Not supported yet.");
            readout.setText(displayInstance.getConv().convertToReal(new ValueFrac32((Double) evt.getNewValue())));
//            readout.setText((Value) evt.getNewValue());
        }
    }
}
