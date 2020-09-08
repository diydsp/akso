package axoloti.swingui.target;

import axoloti.Axoloti;
import axoloti.connection.CConnection;
import axoloti.connection.IConnection;
import axoloti.mvc.IView;
import axoloti.preferences.Preferences;
import axoloti.shell.CompileFirmware;
import axoloti.shell.UploadFirmwareDFU;
import axoloti.target.TargetModel;
import axoloti.usb.Usb;
import java.beans.PropertyChangeEvent;
import java.io.IOException;
import java.util.logging.Level;
import java.util.logging.Logger;
import javax.swing.JMenu;
import javax.swing.JMenuItem;
import javax.swing.JSeparator;

/**
 *
 * @author jtaelman
 */
public class TargetMenu extends JMenu implements IView<TargetModel> {

    final TargetModel targetModel;

    private JMenuItem jMenuItemSelectCom;
    private JMenuItem jMenuItemEnterDFU;
    private JMenuItem jMenuItemFCompile;
    private JMenuItem jMenuItemFConnect;
    private JMenuItem jMenuItemFDisconnect;
    private JMenuItem jMenuItemFlashDFU;
    private JMenuItem jMenuItemFlashDefault;
    private JMenuItem jMenuItemFlashUpgrade_v1_v2;
    private JMenuItem jMenuItemFlashSDR;
    private JMenuItem jMenuItemMount;
    private JMenuItem jMenuItemPing;
    private JMenuItem jMenuItemRefreshFWID;
    private JSeparator jDevSeparator;
    private JMenu jMenuFirmware;
    private JSeparator jSeparator1;
    private JMenuItem jMenuItemKeyboard;
    private JMenuItem jMenuItemMidiMonitor;
    private JMenuItem jMenuItemMidiRouting;
    private JMenuItem jMenuItemFileManager;
    private JMenuItem jMenuItemRemote;

    public TargetMenu(TargetModel targetModel) {
        super("Board");
        this.targetModel = targetModel;
        initComponents();
    }

    private void initComponents() {
        jMenuItemSelectCom = new JMenuItem("Select Device...");
        jMenuItemSelectCom.addActionListener(new java.awt.event.ActionListener() {
            @Override
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                jMenuItemSelectComActionPerformed(evt);
            }
        });
        add(jMenuItemSelectCom);

        jMenuItemFConnect = new JMenuItem("Connect");
        jMenuItemFConnect.addActionListener(new java.awt.event.ActionListener() {
            @Override
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                jMenuItemFConnectActionPerformed(evt);
            }
        });
        add(jMenuItemFConnect);

        jMenuItemFDisconnect = new JMenuItem("Disconnect");
        jMenuItemFDisconnect.addActionListener(new java.awt.event.ActionListener() {
            @Override
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                jMenuItemFDisconnectActionPerformed(evt);
            }
        });
        add(jMenuItemFDisconnect);

        jMenuItemPing = new JMenuItem("Ping");
        jMenuItemPing.setEnabled(false);
        jMenuItemPing.addActionListener(new java.awt.event.ActionListener() {
            @Override
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                jMenuItemPingActionPerformed(evt);
            }
        });
        add(jMenuItemPing);

        jMenuItemMount = new JMenuItem("Enter card reader mode (disconnects editor)");
        jMenuItemMount.addActionListener(new java.awt.event.ActionListener() {
            @Override
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                jMenuItemMountActionPerformed(evt);
            }
        });
        add(jMenuItemMount);

        jMenuFirmware = new javax.swing.JMenu();
        jMenuFirmware.setText("Firmware");

        jMenuItemFlashDefault = new JMenuItem("Flash");
        jMenuItemFlashDefault.addActionListener(new java.awt.event.ActionListener() {
            @Override
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                jMenuItemFlashDefaultActionPerformed(evt);
            }
        });
        jMenuFirmware.add(jMenuItemFlashDefault);

        // auto-detected, we do not need to access this manually
        //jMenuFirmware.add(jMenuItemFlashUpgrade_v1_v2);

        jMenuItemFlashDFU = new JMenuItem("Flash (Rescue)");
        jMenuItemFlashDFU.addActionListener(new java.awt.event.ActionListener() {
            @Override
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                jMenuItemFlashDFUActionPerformed(evt);
            }
        });
        jMenuFirmware.add(jMenuItemFlashDFU);

        jMenuItemRefreshFWID = new JMenuItem("Refresh Firmware ID");
        jMenuItemRefreshFWID.addActionListener(new java.awt.event.ActionListener() {
            @Override
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                jMenuItemRefreshFWIDActionPerformed(evt);
            }
        });
        jMenuFirmware.add(jMenuItemRefreshFWID);

        jDevSeparator = new JSeparator();
        jMenuFirmware.add(jDevSeparator);

        jMenuItemFCompile = new JMenuItem("Compile");
        jMenuItemFCompile.addActionListener(new java.awt.event.ActionListener() {
            @Override
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                jMenuItemFCompileActionPerformed(evt);
            }
        });
        jMenuFirmware.add(jMenuItemFCompile);

        jMenuItemEnterDFU = new JMenuItem("Enter Rescue mode");
        jMenuItemEnterDFU.addActionListener(new java.awt.event.ActionListener() {
            @Override
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                jMenuItemEnterDFUActionPerformed(evt);
            }
        });
        jMenuFirmware.add(jMenuItemEnterDFU);

        jMenuItemFlashSDR = new JMenuItem("Flash (User)");
        jMenuItemFlashSDR.addActionListener(new java.awt.event.ActionListener() {
            @Override
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                jMenuItemFlashSDRActionPerformed(evt);
            }
        });
        jMenuFirmware.add(jMenuItemFlashSDR);

        if (!Preferences.getPreferences().getExpertMode()) {
            jMenuItemRefreshFWID.setVisible(false);
        }

        jMenuItemEnterDFU.setVisible(Axoloti.isDeveloper());
        jMenuItemFlashSDR.setVisible(Axoloti.isDeveloper());
        jMenuItemFCompile.setVisible(Axoloti.isDeveloper());
        jDevSeparator.setVisible(Axoloti.isDeveloper());

        add(jMenuFirmware);

        jSeparator1 = new JSeparator();
        add(jSeparator1);
        jMenuItemKeyboard = new JMenuItem("Virtual Keyboard");
        jMenuItemKeyboard.addActionListener(new java.awt.event.ActionListener() {
            @Override
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                TargetViews.getTargetViews().showKeyboard();
            }
        });
        add(jMenuItemKeyboard);
        jMenuItemMidiMonitor = new JMenuItem("MIDI Input Monitor");
        jMenuItemMidiMonitor.addActionListener(new java.awt.event.ActionListener() {
            @Override
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                TargetViews.getTargetViews().showMidiMonitor();
            }
        });

        jMenuItemMidiRouting = new JMenuItem("MIDI Routing");
        jMenuItemMidiRouting.addActionListener(new java.awt.event.ActionListener() {
            @Override
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                TargetViews.getTargetViews().showMidiRouting();
            }
        });

        jMenuItemFileManager = new JMenuItem("File manager");
        jMenuItemFileManager.addActionListener(new java.awt.event.ActionListener() {
            @Override
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                TargetViews.getTargetViews().showFilemanager();
            }
        });
        add(jMenuItemFileManager);
        add(jMenuItemMidiRouting);
        add(jMenuItemMidiMonitor);
    }

    private void jMenuItemFDisconnectActionPerformed(java.awt.event.ActionEvent evt) {
        getDModel().getConnection().disconnect();
    }

    private void jMenuItemFConnectActionPerformed(java.awt.event.ActionEvent evt) {
        getDModel().getConnection().connect(null);
    }

    private void jMenuItemSelectComActionPerformed(java.awt.event.ActionEvent evt) {
        getDModel().getConnection().selectPort();
    }

    private void jMenuItemPingActionPerformed(java.awt.event.ActionEvent evt) {
        try {
            IConnection conn = CConnection.getConnection();
            conn.transmitPing();
        } catch (IOException ex) {
            Logger.getLogger(TargetMenu.class.getName()).log(Level.SEVERE, null, ex);
        }
    }

    private void jMenuItemRefreshFWIDActionPerformed(java.awt.event.ActionEvent evt) {
        getDModel().updateLinkFirmwareID();
    }

    private void jMenuItemFlashDFUActionPerformed(java.awt.event.ActionEvent evt) {
        if (Usb.isDFUDeviceAvailable()) {
            getDModel().updateLinkFirmwareID();
            UploadFirmwareDFU.doit();
        } else {
            Logger.getLogger(TargetMenu.class.getName()).log(Level.SEVERE, "No attached devices in DFU mode detected. To put Akso in DFU mode: hold switch 1 while powering on. Switch 1 is the switch closest to the two USB-C ports.");
        }
    }

    private void jMenuItemFlashSDRActionPerformed(java.awt.event.ActionEvent evt) {
        String fname = Axoloti.firmwareDirectory() + "/flasher/flasher_build/flasher";
        String pname = Axoloti.firmwareDirectory() + "/build/axoloti.bin";
        getDModel().flashUsingSDRam(fname, pname);
    }

    private void jMenuItemFCompileActionPerformed(java.awt.event.ActionEvent evt) {
        CompileFirmware.doit();
    }

    private void jMenuItemEnterDFUActionPerformed(java.awt.event.ActionEvent evt) {
        final String msg = "Done enabling DFU. The regular USB communication will now abort, " + "and Axoloti Core will restart itself in \"rescue\" (DFU) mode."
                + "\"Flash (rescue)\" will restart Axoloti Core again in normal mode when completed."
                + "To leave \"rescue\" mode, power-cycle your Axoloti Core.";
        try {
            IConnection conn = CConnection.getConnection();
            conn.bringToDFU();
        } catch (IOException ex) {
            Logger.getLogger(TargetMenu.class.getName()).log(Level.SEVERE, null, ex);
        }
    }

    private void jMenuItemFlashDefaultActionPerformed(java.awt.event.ActionEvent evt) {
        String fname = Axoloti.firmwareDirectory() + "/flasher/flasher_build/flasher";
        String pname = Axoloti.firmwareDirectory() + "/build/axoloti.bin";
        getDModel().flashUsingSDRam(fname, pname);
    }

    private void jMenuItemMountActionPerformed(java.awt.event.ActionEvent evt) {
        try {
            String fname = Axoloti.firmwareDirectory() + "/mounter/mounter_build/mounter";
            IConnection conn = CConnection.getConnection();
            conn.transmitStop();
            TargetModel.getTargetModel().uploadPatchToMemory(fname);
            conn.transmitStart();
            Logger.getLogger(TargetMenu.class.getName()).log(Level.SEVERE, "SDCard mounter active, editor connection lost. Unmount/eject the sdcard volume in Explorer or Finder to enable editor connection again.");
        } catch (IOException ex) {
            Logger.getLogger(TargetMenu.class.getName()).log(Level.SEVERE, null, ex);
        }
    }

    @Override
    public void modelPropertyChange(PropertyChangeEvent evt) {
        if (TargetModel.CONNECTION.is(evt)) {
            IConnection connection = (IConnection) evt.getNewValue();
            boolean connect = connection != null;
            jMenuItemFDisconnect.setEnabled(connect);
            jMenuItemFConnect.setEnabled(!connect);
            jMenuItemSelectCom.setEnabled(!connect);
            jMenuItemEnterDFU.setEnabled(connect);
            jMenuItemMount.setEnabled(connect);
            jMenuItemFlashDefault.setEnabled(
                    (connection != null)
                    && connection.getTargetProfile().hasSDRAM());
            jMenuItemFlashSDR.setEnabled(
                    (connection != null)
                    && connection.getTargetProfile().hasSDRAM());
        } else if (TargetModel.HAS_SDCARD.is(evt)) {
            Boolean b = (Boolean) evt.getNewValue();
            jMenuItemMount.setEnabled(b);
        }

    }

    @Override
    public TargetModel getDModel() {
        return targetModel;
    }

    @Override
    public void dispose() {
        targetModel.getController().removeView(this);
    }

}
